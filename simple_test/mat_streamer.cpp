//
// Created by 张义杰 on 2018/3/6.
//
#include <iostream>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <opencv2/core/mat.hpp>

using namespace std;
class codec{
private:
    AVCodec *pcodec= nullptr;
    AVCodecContext *pcodec_ctx=nullptr;
public:
    codec(AVCodecID id){
        pcodec = avcodec_find_encoder(id);
        pcodec_ctx=avcodec_alloc_context3(pcodec);
        avcodec_open2(pcodec_ctx, 0, 0);
        settingCodec();
    }
    int send(AVFrame *frame){
        return avcodec_send_frame(pcodec_ctx,frame);
    }
    int receive(AVPacket *packat){
        return avcodec_receive_packet(pcodec_ctx,packat);
    }
private:
    void settingCodec(){
        pcodec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
        pcodec_ctx->codec_id = pcodec->id;
        pcodec_ctx->thread_count = 8;

        pcodec_ctx->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
        pcodec_ctx->width = 640;
        pcodec_ctx->height = 480;
        pcodec_ctx->time_base = { 1,50 };
        pcodec_ctx->framerate = { 50,1 };

        //画面组的大小，多少帧一个关键帧
        pcodec_ctx->gop_size = 50;
        pcodec_ctx->max_b_frames = 0;
        pcodec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    }
};
int main(int argc, char *argv[]) {
  AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
  if (argc < 2) {
    cout << "usage :" << argv[0] << " [output url] " << endl;
    cout << "example : " << argv[0] << "rtmp://localhost/live/livestream"
         << endl;
  }
  if(avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", argv[1])<0){
    cerr<<"cann't open output context "<<endl;
    exit(1);
  }
    //编码器相关，将frame转换成帧

    codec codec1(AV_CODEC_ID_H264);
    AVStream *out_stream = avformat_new_stream(ofmt_ctx,NULL);
 if(!out_stream){
   cerr<<"cann't open stream "<<endl;
   exit(1);
 }
  out_stream->codecpar->codec_tag =0;
  out_stream->codecpar->codec_id = AV_CODEC_ID_H264;
    out_stream->codecpar->bit_rate =50*1024*8;
    out_stream->codecpar->width = 640;
    out_stream->codecpar->height=480;

    if(!avio_open(&ofmt_ctx->pb,argv[1],AVIO_FLAG_WRITE)){
        cerr<<"cann't open io "<<endl;
        exit(1);
    }
    if(!avformat_write_header(ifmt_ctx, NULL)){
        cerr <<"cann't write header"<<endl;
        exit(1);
    }

    int vpts=0;
    bool running =true;
    SwsContext *vsc=NULL;
    AVFrame *yuv=NULL;
    vsc = sws_getCachedContext(vsc,
                               640, 480, AV_PIX_FMT_BGR24,     //源宽、高、像素格式
                               640, 480, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
                               SWS_BICUBIC,  // 尺寸变化使用算法
                               0, 0, 0
    );
    yuv = av_frame_alloc();
    yuv->format = AV_PIX_FMT_YUV420P;
    yuv->width = 640;
    yuv->height = 480;
    yuv->pts = 0;
    //分配yuv空间
    int ret = av_frame_get_buffer(yuv, 0);
    AVPacket pack;
    memset(&pack,0,sizeof(pack));
    while(running){
        cv::Mat image_data;
        uint8_t *indata[AV_NUM_DATA_POINTERS] ={0};
        indata[0]=image_data.data;
        int insize[AV_NUM_DATA_POINTERS]={0};
        insize[0]=image_data.cols * image_data.elemSize();
        int h=sws_scale(vsc,indata,insize,0,image_data.rows,yuv->data,yuv->linesize);
        if(h<=0)
            continue;
        yuv->pts=vpts;
        vpts++;
        if(!codec1.send(yuv))
        {
            cerr<<"error to send frame"<<endl;
        }
        if(!codec1.receive(&pack)){
            cerr<<"error to receive package"<<endl;
        }
        pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
        pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
        pack.duration = av_rescale_q(pack.duration, vc->time_base, vs->time_base);
        ret = av_interleaved_write_frame(ic, &pack);
    }

end:
  avformat_free_context(ofmt_ctx);

}
