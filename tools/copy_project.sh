####################=========================copy  nv=========================######################
echo $BUILD_TARGET
LOCATION="prebuilts/modem/8850"
C3_8910DM_LOCATION="prebuilts/modem/8910"

nvmdir=`cat target/$BUILD_TARGET/target.config|grep CONFIG_NVM_VARIANTS|awk -F= '{print $2}'`
nvmname=`echo ${nvmdir:1:-1}`
echo "nvm name is ${nvmname}"

if [[ $BUILD_TARGET =~ "8910" ]]
then
    echo $BUILD_TARGET
    cp -rf target/$BUILD_TARGET/nv/* $C3_8910DM_LOCATION/${nvmname}/
else
    echo $BUILD_TARGET
    cp -rf target/$BUILD_TARGET/nv/* $LOCATION/${nvmname}/
fi

echo $LINENO

