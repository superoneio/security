#!/bin/bash

cleos create account eosio eosio.token EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX

echocolor "创建eosio.msig账户"
cleos create account eosio eosio.msig EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX

echocolor "创建eosio.ram账户"
cleos create account eosio eosio.ram EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX

echocolor "创建eosio.ramfee账户"
cleos create account eosio eosio.ramfee EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX

echocolor "创建eosio.stake账户"
cleos create account eosio eosio.stake EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX


#部署token合约
echocolor "部署token合约"
cleos set contract eosio.token eosio.token

#部署msig合约
echocolor "部署msig合约 "
cleos set contract eosio.msig eosio.msig

#创建SYS代币
echocolor "创建SYS代币 "
cleos  push action eosio.token create '["eosio","100000000000.0000 EOS"]' -p eosio.token

#发币
echocolor "发币 "
cleos  push action eosio.token issue  '["eosio","100000000000.0000 EOS","issue"]' -p eosio

#查询数据库余额
echocolor "查询数据库余额 "
cleos get table eosio.token eosio accounts

echo "================================== set system  contract ============================================="

#部署system合约
echocolor "部署system合约 "
cleos  set contract eosio eosio.system

echo "================================== eosio create account ============================================="

#创建账户
echocolor "创建账户 "
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio debug  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 1000 eosio debug2  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX


# 创建测试账号
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio tobetioadmin  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio tobetiologs1  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio debug  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio blacklist  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '0.7000 EOS' --stake-cpu '0.7000 EOS' --buy-ram-kbytes 1000 eosio attack  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX

# 初始化合约
cleos set contract tobetioadmin ./tobetioadmin/
cleos set contract tibetiologs1 ./tibetiologs1/
cleos set contract attack ./attack/

# 初始化余额
cleos push action eosio.token transfer '["eosio","tobetioadmin","1000000.0000 EOS",""]' -p eosio
cleos push action eosio.token transfer '["eosio","blacklist","1000000.0000 EOS",""]' -p eosio
cleos push action eosio.token transfer '["eosio","debug","1000000.0000 EOS",""]' -p eosio
cleos push action eosio.token transfer '["eosio","attack","10000.0000 EOS",""]' -p eosio
 cleos push action tobetioadmin reveal '["debug","2.0000 EOS","96-27-346a0ba3da5388cc4ffc4326c96050661d98c1e57b0d392e6f9271201b7439a8-f8554b2c5b0430e0d890d338292613f7aefc1c33-3770256"]' -p tobetioadmin

# 添加 eosio.code 权限
cleos set account permission tobetioadmin active '{"threshold": 1,"keys":[{"key":"EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX","weight":1}],"accounts": [{"permission":{"actor":"tobetioadmin","permission":"eosio.code",}"weight":1}]}' -p tobetioadmin@owner
cleos set account permission attack active '{"threshold": 1,"keys":[{"key":"EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX","weight":1}],"accounts": [{"permission":{"actor":"attack","permission":"eosio.code",}"weight":1}]}' -p attack@owner

cleos -u http://127.0.0.1:8889 transfer debug tobetioadmin "3.0000 EOS" "96-27-346a0ba3da5388cc4ffc4326c96050661d98c1e57b0d392e6f9271201b7439a8-f8554b2c5b0430e0d890d338292613f7aefc1c33-3770266" -p debug 
cleos -u http://127.0.0.1:8889 transfer blacklist tobetioadmin "3.0000 EOS" "96-27-346a0ba3da5388cc4ffc4326c96050661d98c1e57b0d392e6f9271201b7439a8-f8554b2c5b0430e0d890d338292613f7aefc1c33-3770267" -p blacklist 
cleos -u http://127.0.0.1:8889 transfer blacklist tobetioadmin "3.0000 EOS" "96-27-346a0ba3da5388cc4ffc4326c96050661d98c1e57b0d392e6f9271201b7439a8-f8554b2c5b0430e0d890d338292613f7aefc1c33-3770268" -p blacklist 
cleos -u http://127.0.0.1:8889 transfer blacklist tobetioadmin "3.0000 EOS" "96-27-346a0ba3da5388cc4ffc4326c96050661d98c1e57b0d392e6f9271201b7439a8-f8554b2c5b0430e0d890d338292613f7aefc1c33-3770269" -p blacklist 