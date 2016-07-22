#!/bin/bash
srcdir=$1

dstf=$2

if [ "$srcdir" == "" -o  "$dstf" == "" ] ; then 
	echo "spritepack <srcdir> <dstf> [symname=spack] [tobin=/tftimage1.py] [defpref=spr_]"
	exit
fi
if [ -r $dstf ] ; then
	mv $dstf $dstf.old
fi

symname=$3
if [ "$symname" == "" ] ; then 
	symname='spack'
fi
tobin=$4
if [ "$tobin" == "" ] ; then 
	tobin=./tftimage1.py
fi

defpref=$5
if [ "$defpref" == "" ] ; then 
	defpref="spr_"
fi


prambule="#ifndef HAS_SPRITEPAC_T\n#define HAS_SPRITEPAC_T\n#include <stdint.h>\ntypedef struct {\n\tuint16_t w;\n\tuint16_t h;\n\tuint32_t ofs;\n} spritepac_t;\n#endif //HAS_SPRITEPAC_T"

defines="#define ${defpref}spritepack_file \"`basename $dstf`\"\n"
sprites=""

num=-1;
ofs=0;
tmpbin=`mktemp`
for fn in $srcdir/*.png; do
	if [ ! -r $fn ] ; then
		continue
	fi
	
	spritename=`basename $fn .png | tr " /.-" "_"`
	num=`expr $num + 1`
	
	defines="$defines\n#define ${defpref}${spritename} ${num}\n#define ${defpref}${spritename}_p (${symname}+${num})\n#define ${defpref}${spritename}_w (${symname}[${num}].w)\n#define ${defpref}${spritename}_h (${symname}[${num}].h)\n#define ${defpref}${spritename}_ofs (${symname}[${num}].ofs)\n"
	
	dim=(`identify -format "%w %h" $fn`)
	if [ "$sprites" != "" ]; then
		sep=",\n\t";
	else 
		sep="\t";
	fi
	sprites="$sprites$sep{ ${dim[0]},${dim[1]},${ofs} }"
	$tobin $fn $tmpbin
	
	binsz=`stat -c %s $tmpbin`
	ofs=`expr $ofs + $binsz`
	cat $tmpbin >> $dstf
done
rm -f $tmbin

if [ "$num" != "-1" ] ; then
	num=`expr $num + 1`
	echo -e "$prambule\n$defines\nconst spritepac_t ${symname}[${num}]={\n$sprites\n};"
fi;
