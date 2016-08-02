#!/bin/bash
srcdir=$1

dstf=$2

if [ "$srcdir" == "" -o  "$dstf" == "" ] ; then 
	echo "spritepack <srcdir> <dstf> [symname=spack] [tobin=../png2bin_st7735rc] [defpref=spr_]"
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
	tobin="../png2bin_st7735rc"
fi

defpref=$5
if [ "$defpref" == "" ] ; then 
	defpref="spr_"
fi


prambule="#ifndef HAS_SPRITEPAC_T\n#define HAS_SPRITEPAC_T\n#include <stdint.h>\ntypedef struct {\n\tuint16_t w;\n\tuint16_t h;\n\tuint32_t ofs;\n\tuint8_t isrle;\n} spritepac_t;\n#endif //HAS_SPRITEPAC_T"

defines="#define ${defpref}spritepack_file \"`basename $dstf`\"\n"
sprites=""

num=-1;
ofs=0;
tmpbin=`mktemp`
tmprle=`mktemp`
for fn in $srcdir/*.png; do
	if [ ! -r $fn ] ; then
		continue
	fi
	
	spritename=`basename $fn .png | tr " /.-" "_"`
	num=`expr $num + 1`
	
	defines="$defines\n#define ${defpref}${spritename} ${num}\n#define ${defpref}${spritename}_p (${symname}+${num})\n#define ${defpref}${spritename}_w (${symname}[${num}].w)\n#define ${defpref}${spritename}_h (${symname}[${num}].h)\n#define ${defpref}${spritename}_ofs (${symname}[${num}].ofs)\n"
	resl=`$tobin $fn $tmpbin $tmprle`
	res=($resl)
	if [ "${res[0]}" != "OK!" ] ; then 
		prambule="//!!!!! failed to process $fn err: $resl"
		continue;
	fi

	#echo "w: ${res[2]} h: ${res[4]} rawsz: ${res[6]} rlesz: ${res[8]} resl:$resl"
	
	if [ "${res[8]}" -lt "${res[6]}" ] ; then
		isrle=1
		store=$tmprle
		storesz=${res[8]}
	else 
		isrle=0
		store=$tmpbin
		storesz=${res[6]}
	fi
	
	if [ "$sprites" != "" ] ; then
		sep=",\n\t";
	else 
		sep="\t";
	fi
	sprites="$sprites$sep{ ${res[2]},${res[4]},${ofs}, $isrle}"

	ofs=`expr $ofs + $storesz`
	cat $store >> $dstf
done
rm -f $tmbin
rm -f $tmprle

if [ "$num" != "-1" ] ; then
	num=`expr $num + 1`
	echo -e "$prambule\n$defines\nconst spritepac_t ${symname}[${num}]={\n$sprites\n};"
fi;
