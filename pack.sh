#!/usr/bin/env bash
# Executable packer
#
# Going_digital 2012
#
# Splices in a short decompress script and leverages the compressors shipped
# in the Raspbian Wheezy distro
#
# Compression effort set so compile is possible on a 256M RasPi

# GZIP method
#echo "a=/tmp/I;tail -n+2 \$0|zcat>\$a;chmod +x \$a;\$a;exit" > gz$1
#cat $1 | gzip -c >> gz$1
#chmod +x gz$1

# BZIP2 method
#echo "a=/tmp/I;tail -n+2 \$0|bzcat>\$a;chmod +x \$a;\$a;rm \$a;exit" > bz$1
#cat $1 | bzip2 -z -8 >> bz$1
#chmod +x bz$1

# XZ method
#echo "a=/tmp/I;tail -n+2 \$0|xzcat>\$a;chmod +x \$a;\$a;rm \$a;exit" > xz$1
#cat $1 | xz -z -C none -7 >> xz$1
#chmod +x xz$1

# LZMA method (seems to be consistently the best for 4k's)
# Decompression stub overhead 57 bytes
echo "a=/tmp/I;tail -n+2 \$0|lzcat>\$a;chmod +x \$a;\$a;rm \$a;exit" > $1.pack
cat $1 | lzma -v -v -z -C none -7 >> $1.pack
chmod +x $1.pack
