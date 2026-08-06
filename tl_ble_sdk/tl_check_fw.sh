echo "************************* start of post build *************************"

os=$(uname)
echo "OS from uname is: ${os}"

if [ "${os}" = "Linux" ] ; then
    echo "Linux OS"
    echo "check_fw in Linux..."
    chmod 755 ../../../check_fw
    ../../../check_fw $1.bin
else
    echo "Windows OS"
    echo "check_fw in Windows..."
    ../../../tl_check_fw2.exe  $1.bin
fi

echo  "-------------------- SDK version info --------------------"
str=$(grep -E "[\$]{3}[a-zA-Z0-9 _.]+[\$]{3}" --text -o $1.bin | sed 's/\$//g')
if [ -z "$str" ]; 
    then echo "no SDK version found at the end of firmware, please check sdk_version.c and sdk_version.h"
else 
    echo "$str";
fi
echo  "-------------------- SDK version end  --------------------"

filesize=$(stat --format=%s $1.bin)
if [ $filesize -gt 262144 ] ; then
    echo "bin size is greater than 256KB, please refer to handbook!"
fi

echo "************************** end of post build **************************"
echo "this is post build!! current configure is :$1"