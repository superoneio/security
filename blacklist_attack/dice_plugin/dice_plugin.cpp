#include <eosio/dice_plugin/dice_plugin.hpp>

#include <eosio/chain/global_property_object.hpp>
#include <fc/io/json.hpp>
#include <fc/utf8.hpp>
#include <fc/variant.hpp>

#include <boost/algorithm/string.hpp>

namespace eosio {

    using namespace eosio::chain;

    static appbase::abstract_plugin& _timing_plugin = app().register_plugin<dice_plugin>();

    class dice_plugin_impl : public std::enable_shared_from_this<dice_plugin_impl>{
        public:
            dice_plugin_impl(){}
            ~dice_plugin_impl(){}

            uint32_t max_action_size_per_trx;
            chain_plugin* chain_plug = nullptr;
            std::set<string> gameid;
            name dice_name;
            name dice_permission_name;
            name dice_contract_name;
            name dice_action_name;
            using signature_provider_type = std::function<chain::signature_type(chain::digest_type)>;
            signature_provider_type dice_seed_provider;
            signature_provider_type dice_signature_provider;
            fc::optional<boost::signals2::scoped_connection> bet_tranaction;

            vector<char> json_to_bin(abi_serializer abis, name code, name action, fc::variant args){
                vector<char> binargs;
                auto action_type = abis.get_action_type(action);
                EOS_ASSERT(!action_type.empty(), action_validate_exception, "Unknown action ${action} in contract ${contract}", ("action", action)("contract", code));
                try {
                 binargs = abis.variant_to_binary(action_type, args, chain_plug->get_abi_serializer_max_time());
                } EOS_RETHROW_EXCEPTIONS(chain::invalid_action_args_exception,
                                        "'${args}' is invalid args for action '${action}' code '${code}'. expected '${proto}'",
                                        ("args", args)("action", action)("code", code)("proto", "hehe"))
                return binargs;
            }

            void get_reveal_action(abi_serializer abis, signed_transaction& trx,fc::variant_object args){
                    // ilog("${sig}",("sig",sig));
                    action reveal_action;
                    reveal_action.account = dice_contract_name;
                    reveal_action.name = dice_action_name;
                    reveal_action.authorization = vector<permission_level>{{dice_name,dice_permission_name}};
                    reveal_action.data = json_to_bin(abis,dice_contract_name,dice_action_name,args);
                    trx.actions.emplace_back( std::move(reveal_action) );
            }

            signed_transaction get_reveal_transaction(fc::variant_object args){
                signed_transaction trx;
                // list action
                const auto &d = chain_plug->chain().db();
                const account_object *code_accnt = d.find<account_object, by_name>(dice_contract_name);
                EOS_ASSERT(code_accnt != nullptr, chain::account_query_exception, "Fail to retrieve account for ${account}", ("account", dice_contract_name) );
                abi_def abi;
                if( abi_serializer::to_abi(code_accnt->abi, abi) ){
                    abi_serializer abis( abi, chain_plug->get_abi_serializer_max_time() );
                    get_reveal_action(abis,trx,args);
                } else {
                    EOS_ASSERT(false, abi_not_found_exception, "No ABI found for ${contract}", ("contract", dice_contract_name));
                }
                
                trx.expiration = chain_plug->chain().pending_block_time() + fc::microseconds(999'999);
                trx.set_reference_block(chain_plug->chain().head_block_id());
                trx.signatures.push_back(dice_signature_provider(trx.sig_digest(chain_plug->chain().get_chain_id(), trx.context_free_data)));
                return trx;
            }

            void exec_reveal_transaction(fc::variant_object args){
                try{
                    auto ontrx = packed_transaction( get_reveal_transaction(args) );
                    if( ontrx.get_signed_transaction().actions.size() == 0 ){
                        return ;
                    }
                    // auto ptr = chain_plug->chain().push_transaction(ontrx, fc::time_point::maximum(), chain_plug->chain().get_global_properties().configuration.min_transaction_cpu_usage );
                    chain_plug->accept_transaction(ontrx, [=](const static_variant<fc::exception_ptr, transaction_trace_ptr>& result) {
                         if (result.contains<fc::exception_ptr>()) {
                            ilog("bad packed_transaction : ${m}", ("m",result.get<fc::exception_ptr>()->what()));
                         } else {
                            auto trace = result.get<transaction_trace_ptr>();
                            if (!trace->except) {
                               ilog("chain accepted transaction");
                               return;
                            }

                            ilog( "bad packed_transaction : ${m}", ("m",trace->except));
                         }
                      });
                    

                    // if(ptr->except){
                    //     wlog("transaction failed ${err}",("err",ptr->except));
                    // }
                } catch( const boost::interprocess::bad_alloc& e  ) {
                 elog( "on block transaction failed due to a bad allocation" );
                 throw;
                } catch( const fc::exception& e ) {
                    wlog( "on block transaction failed, but shouldn't impact block generation, system contract needs update" );
                    edump((e.to_detail_string()));
                } catch( const std::exception& e ) {
                    wlog( "on block transaction failed, but shouldn't impact block generation, system contract needs update" );
                    wlog("${err}",("err",e.what()));
                }
            }

            void handler_transaction(const chain::transaction_trace_ptr& tt){
                dfs_inline_traces(tt->action_traces );
            }

            void dfs_inline_traces( vector<chain::action_trace> trace ){
                for(auto& atc : trace){
                    if( atc.receipt.receiver == atc.act.account ){
                        if( atc.act.account.to_string() == "eosio.token" ){
                            const auto &d = chain_plug->chain().db();
                            const account_object *code_accnt = d.find<account_object, by_name>(N(eosio.token));
                            EOS_ASSERT(code_accnt != nullptr, chain::account_query_exception, "Fail to retrieve account for ${account}", ("account", dice_contract_name) );
                            abi_def abi;
                            if( abi_serializer::to_abi(code_accnt->abi, abi) ){
                                abi_serializer abis( abi, chain_plug->get_abi_serializer_max_time() );
                                auto data = abis.binary_to_variant( abis.get_action_type( atc.act.name ), atc.act.data, chain_plug->get_abi_serializer_max_time() );
                                if( data["to"] == dice_contract_name.to_string()){
                                    ilog("${data}",("data",data));
                                    std::vector<std::string> transfer_memo;
                                    string memo = data["memo"].as<std::string>();
                                    boost::split( transfer_memo, memo, boost::is_any_of( "-" ));
                                    ilog("${memo}",("memo",transfer_memo));
                                    if( gameid.find(transfer_memo[transfer_memo.size()-1]) == gameid.end() ){
                                        gameid.insert(transfer_memo[transfer_memo.size()-1]);
                                        exec_reveal_transaction( fc::mutable_variant_object()("player",data["from"])("quantity",data["quantity"])("memo",data["memo"]));
                                    }
                                    
                                }
                            } else {
                                EOS_ASSERT(false, abi_not_found_exception, "No ABI found for ${contract}", ("contract", dice_contract_name));
                            }
                        }
                        if( atc.inline_traces.size()!=0 ){
                            dfs_inline_traces( atc.inline_traces );
                        }
                    }
                }
            }

            template<typename Function, typename Function2>
            void walk_key_value_table(const name& code, const name& scope, const name& table, Function f, Function2 f2) const {
                const auto& d = chain_plug->chain().db();
                const auto* t_id = d.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple(code, scope, table));
                if (t_id != nullptr) {
                    const auto &idx = d.get_index<chain::key_value_index, chain::by_scope_primary>();
                    decltype(t_id->id) next_tid(t_id->id._id + 1);
                    auto lower = idx.lower_bound(boost::make_tuple(t_id->id));
                    auto upper = idx.lower_bound(boost::make_tuple(next_tid));
                    for (auto itr = lower; itr != upper; ++itr) {
                        if (!f(*itr)) {
                            break;
                        }
                    }
                } else {
                    f2();
                }
            }

    };

    static dice_plugin_impl::signature_provider_type make_key_signature_provider(const private_key_type& key) {
       return [key]( const chain::digest_type& digest ) {
          return key.sign(digest);
       };
    }

    dice_plugin::dice_plugin():my(new dice_plugin_impl()){}
    dice_plugin::~dice_plugin(){}

    void dice_plugin::set_program_options(options_description& cli, options_description& cfg){
        dlog("set_program_options");

        cfg.add_options()
                ("dice-name", bpo::value<std::string>()->default_value(""),
                "contract for dice.")
                ("dice-permission-name", bpo::value<std::string>()->default_value(""),
                "permission name for dice.")
                ("dice-contract-name", bpo::value<std::string>()->default_value(""),
                "Key=Value for dice.")
                ("dice-action-name", bpo::value<std::string>()->default_value(""),
                "Key=Value for dice.")
                ("dice-signature-provider", boost::program_options::value<vector<string>>()->composing()->multitoken(),
                "Key=Value for dice.")
                ;
    }

    void dice_plugin::plugin_initialize(const variables_map& options) {

        if( options.count("dice-name") ){
            my->dice_name = options["dice-name"].as<std::string>();
        }

        if( options.count("dice-permission-name") ){
            my->dice_permission_name = options["dice-permission-name"].as<std::string>();
        }

        if( options.count("dice-contract-name") ){
            my->dice_contract_name = options["dice-contract-name"].as<std::string>();
        }

        if( options.count("dice-action-name") ){
            my->dice_action_name = options["dice-action-name"].as<std::string>();
        }

        if( options.count("dice-signature-provider") ) {
          const std::vector<std::string> key_spec_pairs = options["dice-signature-provider"].as<std::vector<std::string>>();
          for (const auto& key_spec_pair : key_spec_pairs) {
             try {
                auto delim = key_spec_pair.find("=");
                EOS_ASSERT(delim != std::string::npos, plugin_config_exception, "Missing \"=\" in the key spec pair");
                auto pub_key_str = key_spec_pair.substr(0, delim);
                auto pri_key_str = key_spec_pair.substr(delim + 1);

                auto pubkey = public_key_type(pub_key_str);
                auto prikey = private_key_type(pri_key_str);
                my->dice_signature_provider = make_key_signature_provider(prikey);

             } catch (...) {
                elog("Malformed signature provider: \"${val}\", ignoring!", ("val", key_spec_pair));
             }
          }
       }

       my->chain_plug = app().find_plugin<chain_plugin>();
       auto& chain = my->chain_plug->chain();

       my->bet_tranaction.emplace(chain.applied_transaction.connect([this](const chain::transaction_trace_ptr& tt) {
            my->handler_transaction(tt);
       } ));

    }

    void dice_plugin::plugin_startup(){
    }

    void dice_plugin::plugin_shutdown(){
        
    }

}