#include "framebuffer.h"

namespace ospray {
  
  FrameBuffer *createLocalFB_RGBA_I8(const vec2i &size)
  {
    Assert(size.x > 0);
    Assert(size.y > 0);
    return new LocalFrameBuffer<unsigned int>(size);
  }
}
