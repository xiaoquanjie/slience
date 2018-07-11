#!/bin/sh

#source ./thirdparty_library/link.sh
cp -r thirdparty_library/ ~/

lib1=~/thirdparty_library/mysql/linux_lib64/
lib2=~/thirdparty_library/protobuf/linux
lib_path=$lib1:$lib2

dst_file=~/.bash_profile

grep 'export LIBRARY_PATH' $dst_file
if [ $? -ne 0 ];then
  echo adding.....
  echo "export LIBRARY_PATH=$lib_path" >> $dst_file
else
  echo changing.....
  sed -i "s%export LIBRARY_PATH=.*%export LIBRARY_PATH=$lib_path%g" $dst_file
fi

grep 'export LD_LIBRARY_PATH' $dst_file
if [ $? -ne 0 ];then
  echo adding.....
  echo "export LD_LIBRARY_PATH=$lib_path" >> $dst_file
else
  echo changing.....
  sed -i "s%export LD_LIBRARY_PATH=.*%export LD_LIBRARY_PATH=$lib_path%g" $dst_file
fi


source $dst_file

