#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <appbase/application.hpp>

namespace eosio{

    class dice_plugin : public plugin<dice_plugin>{
        public:

            dice_plugin();
            virtual ~dice_plugin();

            APPBASE_PLUGIN_REQUIRES((chain_plugin));
            virtual void set_program_options(options_description& cli, options_description& cfg) override;

            virtual void plugin_initialize(const variables_map& options);
            virtual void plugin_startup();
            virtual void plugin_shutdown();

        private:
            std::shared_ptr<class dice_plugin_impl> my;

    };

}