TOTEST=524288

#ResNet 18
OUT=evaluation/Resnet-View-0-of-14-A12-Rev-11-MISP/
mkdir -p $OUT

./pretrain-d.sh --ferev 11 --elu \
	--ckptdir ackpt/Resnet-View-0-of-14-Action12-Rev-11-Feat-2048-524288 \
	--ckptprefix working-try11 \
	--batch 2 --queuemax 64 --threads 1 \
	--avi \
	--res 224 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--eval \
	--view 0 \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > evaluation/Resnet-View-0-of-14-Rev-11-Feat-2048-524288.out

exit
# Evaluate naive rev 5

OUT=evaluation/View-0-of-14-A12-Rev-5-MISP/
mkdir -p $OUT

./pretrain-d.sh --ferev 5 --elu \
	--ckptdir ackpt/View-0-of-14-Action12-Rev-5-Feat-2048-524288 --ckptprefix working-try11 \
	--batch 2 --queuemax 64 --threads 1 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--eval \
	--view 0 \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > $OUT/stdout

# More training iteratinos

OUT=evaluation/View-0-of-14-A12-Rev-5-MISP-Pass2/
mkdir -p $OUT

./pretrain-d.sh --ferev 5 --elu \
	--eval \
	--ckptdir ackpt/View-0-of-14-Action12-Rev-5-Feat-2048-524288-Pass2/ \
	--ckptprefix working- \
	--batch 2 --queuemax 64 --threads 1 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--view 0 \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > $OUT/stdout

# Larger action magnitude (8x)

OUT=evaluation/View-0-of-14-A12-Rev-5-MISP-8thAMAG/
mkdir -p $OUT

./pretrain-d.sh --ferev 5 --elu \
	--eval \
	--ckptdir ackpt/View-0-of-14-Action12-Rev-5-Feat-2048-524288-8thAMAG \
	--ckptprefix working-try11 \
	--batch 2 --queuemax 64 --threads 1 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--view 0 \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M-8thAMAG/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > $OUT/stdout

# AVI

OUT=evaluation/AVI-View-0-of-14-A12-Rev-5-MISP/
mkdir -p $OUT

./pretrain-d.sh --ferev 5 --elu \
	--eval \
	--ckptdir ackpt/AVI-View-0-of-14-Action12-Rev-5-Feat-2048-524288 \
	--ckptprefix working- \
	--batch 2 --queuemax 64 --threads 1 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--view 0 \
	--avi \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > $OUT/stdout

# 224 x 224

OUT=evaluation/Res-224-View-0-of-14-A12-Rev-10-MISP/
mkdir -p $OUT

./pretrain-d.sh --ferev 10 --elu \
	--eval \
	--ckptdir ackpt/Res-224-View-0-of-14-Action12-Rev-10-Feat-2048-524288 \
       	--ckptprefix working- \
	--batch 2 --queuemax 64 --threads 1 \
	--iter $TOTEST \
	--mispout $OUT \
	--norgbd \
	--view 0 \
	--res 224 \
	--featnum 2048 \
	--imhidden 2048 2048 \
	--samplein sample/batch2-view14-norgbd-T6-R6-2M/ \
	--sampletouse $TOTEST \
	--samplebatching 64 \
	--samplebase 1048576 > $OUT/stdout
