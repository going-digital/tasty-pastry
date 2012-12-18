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
#echo "a=/tmp/I;cp \$0 \$a;tail -n+2 \$0|zcat>\$a;\$a;rm \$a;exit" > $1.pack
#cat $1 | gzip -c >> $1.pack
#chmod +x $1.pack

# BZIP2 method
#echo "a=/tmp/I;cp \$0 \$a;tail -n+2 \$0|bzcat>\$a;\$a;rm \$a;exit" > $1.pack
#cat $1 | bzip2 -z -8 >> $1.pack
#chmod +x $1.pack

# XZ method
#echo "a=/tmp/I;cp \$0 \$a;tail -n+2 \$0|xzcat>\$a;\$a;rm \$a;exit" > $1.pack
#cat $1 | xz -z -C none -7 >> $1.pack
#chmod +x $1.pack

# LZMA method (seems to be consistently the best for 4k's)
# Decompression stub overhead 54 bytes
echo "a=/tmp/I;cp \$0 \$a;tail -n+2 \$0|lzcat>\$a;\$a;rm \$a;exit" > $1.pack
cat $1 | lzma -v -v -z -C none -7 >> $1.pack
chmod +x $1.pack
