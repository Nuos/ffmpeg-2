#include <iostream>
#include <opencv2/core/mat.hpp>
#include <string>
#include <opencv2/imgproc.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}
#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)

#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER

using namespace std;
string out_filename = "rtmp://localhost/live/livestream";
static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   AVFormatContext *ofmt_ctx, AVStream *out_stream) {
  int ret;

  /* send the frame to the encoder */
  if (frame)
    printf("Send frame %3" PRId64 "\n", frame->pts);

  ret = avcodec_send_frame(enc_ctx, frame);
  if (ret < 0) {
    fprintf(stderr, "Error sending a frame for encoding\n");
    exit(1);
  }
  while (ret >= 0) {
    ret = avcodec_receive_packet(enc_ctx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      return;
    else if (ret < 0) {
      fprintf(stderr, "Error during encoding\n");
      exit(1);
    }

    printf("Write packet %3" PRId64 " (size=%5d)\n", pkt->pts, pkt->size);
    pkt->stream_index = out_stream->index;
    if (av_interleaved_write_frame(ofmt_ctx, pkt) < 0) {
      cout << " write frame to file failed" << endl;
    }
    av_packet_unref(pkt);
  }
}
int main(int argc, char *argv[]) {

  AVFormatContext *ofmt_ctx = NULL;
  int i, ret, x, y;
  if (avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv",
                                     out_filename.c_str()) < 0) {
    cout << " alloc output context failed" << endl;
  } // RTMP

  AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
  AVCodecContext *c = avcodec_alloc_context3(codec);

  /* put sample parameters */
  c->bit_rate = 400000;
  /* resolution must be a multiple of two */
  c->width = 640;
  c->height = 480;
  /* frames per second */
  c->time_base = (AVRational){1, 25};
  c->framerate = (AVRational){25, 1};

  /* emit one intra frame every ten frames
   * check frame pict_type before passing frame
   * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
   * then gop_size is ignored and the output of encoder
   * will always be I frame irrespective to gop_size
   */
  c->gop_size = 10;
  c->max_b_frames = 1;
  c->pix_fmt = AV_PIX_FMT_YUV420P;
  if (codec->id == AV_CODEC_ID_H264)
    av_opt_set(c->priv_data, "preset", "slow", 0);
  ret = avcodec_open2(c, codec, NULL);

  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }
  frame->format = c->pix_fmt;
  frame->width = c->width;
  frame->height = c->height;
  ret = av_frame_get_buffer(frame, 0);

  AVPacket *pkt = av_packet_alloc();

  AVStream *out_stream = avformat_new_stream(ofmt_ctx, codec);

  avcodec_parameters_from_context(out_stream->codecpar, c);
  out_stream->codecpar->codec_tag = 0;
  if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;
  std::cout << "out_stream time-base" << out_stream->time_base.den << " "
            << out_stream->time_base.num << endl;
  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    ret = avio_open(&ofmt_ctx->pb, out_filename.c_str(), AVIO_FLAG_WRITE);
  }
  if (avformat_write_header(ofmt_ctx, NULL) < 0) {
    cout << " write header failed" << endl;
  }
  cv::Mat input1(480, 640, CV_8UC3, cv::Scalar(0, 255, 0));
  cv::Mat output1;
  cv::cvtColor(input1,output1,CV_BGR2YUV_I420);
  for (i = 0; i < 3000; i++) {
    fflush(stdout);
    /* make sure the frame data is writable */
    ret = av_frame_make_writable(frame);
    av_image_fill_arrays(frame->data, frame->linesize, output1.data,
                         AV_PIX_FMT_YUV420P, 640, 480, 1);

    frame->pts = i;
    /* encode the image */
    encode(c, frame, pkt, ofmt_ctx, out_stream);
  }

  if (av_write_trailer(ofmt_ctx) < 0) {
    cout << "write trailer failed" << endl;
  }
  avcodec_free_context(&c);
  av_frame_free(&frame);
  av_packet_free(&pkt);
  avformat_free_context(ofmt_ctx);
}