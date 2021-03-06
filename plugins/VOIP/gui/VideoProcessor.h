#pragma once

#include <stdint.h>
#include <QImage>
#include "interface/rsVOIP.h"

class QVideoOutputDevice ;

// This class decodes video from a stream. It keeps a queue of
// decoded frame that needs to be retrieved using the getNextImage() method.
//
class VideoDecoder
{
	public:
		VideoDecoder() ;
		virtual ~VideoDecoder() {}

		// Gets the next image to be displayed. Once returned, the image should
		// be cleared from the incoming queue.
		//
		void setDisplayTarget(QVideoOutputDevice *odev) { _output_device = odev ; }

		virtual void receiveEncodedData(const unsigned char *data,uint32_t size) ;

		// returns the current (measured) frame rate in bytes per second.
		//
		uint32_t currentFrameRate() const; 

	private:
		QVideoOutputDevice *_output_device ;

		std::list<QImage> _image_queue ;

		// Incoming data is processed by a video codec and converted into images.
		//
		virtual QImage decodeData(const unsigned char *encoded_image,uint32_t encoded_image_size) = 0 ;

//		// This buffer accumulated incoming encoded data, until a full packet is obtained,
//		// since the stream might not send images at once. When incoming images are decoded, the
//		// data is removed from the buffer.
//		//
//		unsigned char *buffer ;
//		uint32_t buffer_size ;
};

// This class encodes video using a video codec (possibly homemade, or based on existing codecs)
// and produces a data stream that is sent to the network transfer service (e.g. p3VOIP).
//
class VideoEncoder
{
	public:
		VideoEncoder() {}
		virtual ~VideoEncoder() {}

		// Takes the next image to be encoded. 
		//
		bool addImage(const QImage& Image) ;

		bool packetReady() const { return !_out_queue.empty() ; }
		bool nextPacket(RsVOIPDataChunk& ) ;

		// Used to tweak the compression ratio so that the video can stream ok.
		//
		void setMaximumFrameRate(uint32_t bytes_per_second) ;

	protected:
		//virtual bool sendEncodedData(unsigned char *mem,uint32_t size) = 0 ;
		virtual void encodeData(const QImage& image) = 0 ;

		std::list<RsVOIPDataChunk> _out_queue ;
};

// Now derive various image encoding/decoding algorithms.
//

class JPEGVideoDecoder: public VideoDecoder
{
	protected:
		virtual QImage decodeData(const unsigned char *encoded_image,uint32_t encoded_image_size) ;
};

class JPEGVideoEncoder: public VideoEncoder
{
	public:
		JPEGVideoEncoder() {}

	protected:
		virtual void encodeData(const QImage& Image) ;
};

