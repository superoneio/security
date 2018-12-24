# DICE_PLUGIN

```
copy -rv dice_plugin eos_dir/plugins/
```

```
vim eos_dir/plugins/CMakeLists.txt
add_subdirectory(dice_plugin)
```

```
vim eos_dir/programs/nodeos/CMakeLists.txt
PRIVATE -Wl,${whole_archive_flag} dice_plugin    -Wl,${no_whole_archive_flag}
```

```
./eosio_build.sh
```

```
// the account who exec reveal action
dice-name = tobetioadmin
// exec reveal action permission
dice-permission-name = active
// dice contract name
dice-contract-name = tobetioadmin
// action name which need to exec
dice-action-name = reveal
// dice-name's key,  pub_key=pri_key
dice-signature-provider = public_key=private_key
```
