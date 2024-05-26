/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

void clearImage(Image*img, uint32_t w, uint32_t h,glm::vec4 value){
  for(uint32_t y=0;y<h;++y){
    for(uint32_t x=0;x<w;++x){
      uint8_t*ptr = (uint8_t*)img->data;
      ptr += y*img->pitch + x*img->bytesPerPixel;
      if(img->format == Image::UINT8){
        for(uint32_t i=0;i<img->channels;++i)
          ptr[i] = (uint8_t)value[img->channelTypes[i]]*255.f; 
      }
      if(img->format == Image::FLOAT32){
        float*ptr2 = (float*)ptr;
        for(uint32_t i=0;i<img->channels;++i)
          ptr2[i] = value[img->channelTypes[i]]; 
      }
    }
  }
}

void clear(Framebuffer*fbo,ClearCommand cc){
  if(cc.clearColor && fbo->color.data){
    clearImage(&fbo->color,fbo->width,fbo->height,cc.color);
  }
  if(cc.clearDepth && fbo->depth.data){
    clearImage(&fbo->depth,fbo->width,fbo->height,glm::vec4(cc.depth));
  }
} 

//! [izg_enqueue]
void izg_enqueue(GPUMemory&mem,CommandBuffer const&cb){
  (void)mem;
  (void)cb;

  for(uint32_t i=0;i<cb.nofCommands;i++){
    if(cb.commands[i].type == CommandType::CLEAR){
      ClearCommand cc = cb.commands[i].data.clearCommand;
      Framebuffer*fbo = mem.framebuffers+mem.activatedFramebuffer;
      clear(fbo,cc); 
    }
    if(cb.commands[i].type == CommandType::BIND_FRAMEBUFFER){
      BindFramebufferCommand cc = cb.commands[i].data.bindFramebufferCommand;
      mem.activatedFramebuffer = cc.id;
    }
    if(cb.commands[i].type == CommandType::BIND_PROGRAM){
      BindProgramCommand cc = cb.commands[i].data.bindProgramCommand;
      mem.activatedProgram = cc.id;
    }
    if(cb.commands[i].type == CommandType::BIND_VERTEXARRAY){
      BindVertexArrayCommand cc = cb.commands[i].data.bindVertexArrayCommand;
      mem.activatedVertexArray = cc.id;
    }
    if(cb.commands[i].type == CommandType::DRAW){
      DrawCommand cc = cb.commands[i].data.drawCommand;
      for(uint32_t gl_VertexID=0; gl_VertexID < cc.nofVertices; ++gl_VertexID){
        InVertex inV;
        inV.gl_VertexID = gl_VertexID;
        OutVertex outV;
        ShaderInterface si;
        si.gl_DrawID = mem.gl_DrawID;
        mem.programs[mem.activatedProgram].vertexShader(outV, inV,si);
      }
      mem.gl_DrawID++;
    }
    if(cb.commands[i].type == CommandType::SET_DRAW_ID){
      SetDrawIdCommand cc = cb.commands[i].data.setDrawIdCommand;
      mem.gl_DrawID = cc.id;
    }
  }
  /// \todo Tato funkce reprezentuje funkcionalitu grafické karty.<br>
  /// Měla by umět zpracovat command buffer, čistit framebuffer a kresli.<br>
  /// mem obsahuje paměť grafické karty.
  /// cb obsahuje command buffer pro zpracování.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [izg_enqueue]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto&img = texture.img;
  auto uv1 = glm::fract(glm::fract(uv)+1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture,pix);
}

/**
 * @brief This function reads color from texture with clamping on the borders.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_textureClamp(Texture const&texture,glm::vec2 uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto&img = texture.img;
  auto uv1 = glm::clamp(uv,0.f,1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture,pix);
}

/**
 * @brief This function fetches color from texture.
 *
 * @param texture texture
 * @param pix integer coorinates
 *
 * @return color 4 floats
 */
glm::vec4 texelFetch(Texture const&texture,glm::uvec2 pix){
  auto&img = texture.img;
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  if(pix.x>=texture.width || pix.y >=texture.height)return color;
  if(img.format == Image::UINT8){
    auto colorPtr = (uint8_t*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]]/255.f;
  }
  if(texture.img.format == Image::FLOAT32){
    auto colorPtr = (float*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]];
  }
  return color;
}

