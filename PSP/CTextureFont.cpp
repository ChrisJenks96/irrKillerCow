#include "CTextureFont.hpp"
#include "..\os.h"
//By brick , based on irr gui font loader
namespace engine
{
	namespace ExtModules
	{
		CTextureFont::CTextureFont()
		: m_ActiveDriver(0), m_PositionData(382),
		  m_TextureRef(0), m_WrongChar(0)
		  {
		  }
		  
		CTextureFont::CTextureFont(video::IVideoDriver* driver)
		: /*m_ActiveDriver(driver),*/ m_PositionData(382),
		  m_TextureRef(0), m_WrongChar(0)
		  {
			setDriver(driver);
		  }//end constructor
		  
		CTextureFont::~CTextureFont()
		{
			unload();
		}//end destructor

		void CTextureFont::unload()
		{
				if (m_TextureRef)
				{
					if(m_ActiveDriver)
					{
						m_ActiveDriver->removeTexture(m_TextureRef);
						m_ActiveDriver->drop();
					}	
				}
				
			m_TextureRef   = 0;
			m_ActiveDriver = 0;
			
		}
		
		void CTextureFont::setDriver(video::IVideoDriver* driver)
		{
			m_ActiveDriver = driver;
			
			if(m_ActiveDriver)
				m_ActiveDriver->drop();
			
			m_ActiveDriver->grab();
			
		}
		
		//private helper funcs IMPL 
		void CTextureFont::readPositions16bit(video::ITexture* texture, s32& lowerRightPos)
		{
			s32 pitch = texture->getPitch();
			core::dimension2d<s32> size = texture->getOriginalSize();

			s32* p = (s32*)texture->lock();
			
			if (!p)
				return;

			s32 colorTopLeft = *p;
			s32 colorLowerRight = *(p+1);
			s32 colorBackGround = *(p+2);
			s32 colorBackGroundWithAlphaTrue = (0xFF<<24) | (~(0xFF<<24) & colorBackGround);
			s32 colorBackGroundWithAlphaFalse = (0x00<<24) | (~(0xFF<<24) & colorBackGround);
			s32 colorFont = (0xFF<<24) | (~(0xFF<<24) & 0x00FFFFFF);
			*(p+1) = colorBackGround;
			*(p+2) = colorBackGround;
			core::position2d<s32> pos(0,0);
			c8* row = (c8*)((void*)p);

			for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
			{
				p = (s32*)((void*)row);

				for (pos.X=0; pos.X<size.Width; ++pos.X)
				{
					if (*p == colorTopLeft)
					{
						*p = colorBackGroundWithAlphaFalse;
						m_PositionData.push_back(core::rect<s32>(pos, pos));
					}
					else
					if (*p == colorLowerRight)
					{
						if (m_PositionData.size()<=(u32)lowerRightPos)
						{
							texture->unlock();
							lowerRightPos = 0;
							return;
						}
						*p = colorBackGroundWithAlphaFalse;
						m_PositionData[lowerRightPos].LowerRightCorner = pos;
						++lowerRightPos;
					}
					else
					if (*p == colorBackGround)
						*p = colorBackGroundWithAlphaFalse;
					else
						*p = colorFont;
					++p;
				}
				row += pitch;
			}
			texture->unlock();
		}//end read16
		
		void CTextureFont::readPositions32bit(video::ITexture* texture, s32& lowerRightPos)
		{
			s32 pitch = texture->getPitch();
			core::dimension2d<s32> size = texture->getOriginalSize();

			s32* p = (s32*)texture->lock();
			
			if (!p)
				return;

			s32 colorTopLeft = *p;
			s32 colorLowerRight = *(p+1);
			s32 colorBackGround = *(p+2);
			s32 colorBackGroundWithAlphaTrue = (0xFF<<24) | (~(0xFF<<24) & colorBackGround);
			s32 colorBackGroundWithAlphaFalse = (0x00<<24) | (~(0xFF<<24) & colorBackGround);
			s32 colorFont = (0xFF<<24) | (~(0xFF<<24) & 0x00FFFFFF);
			*(p+1) = colorBackGround;
			*(p+2) = colorBackGround;
			core::position2d<s32> pos(0,0);
			c8* row = (c8*)((void*)p);

			for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
			{
				p = (s32*)((void*)row);

				for (pos.X=0; pos.X<size.Width; ++pos.X)
				{
					if (*p == colorTopLeft)
					{
						*p = colorBackGroundWithAlphaFalse;
						m_PositionData.push_back(core::rect<s32>(pos, pos));
					}
					else
					if (*p == colorLowerRight)
					{
						if (m_PositionData.size()<=(u32)lowerRightPos)
						{
							texture->unlock();
							lowerRightPos = 0;
							return;
						}
						*p = colorBackGroundWithAlphaFalse;
						m_PositionData[lowerRightPos].LowerRightCorner = pos;
						++lowerRightPos;
					}
					else
					if (*p == colorBackGround)
						*p = colorBackGroundWithAlphaFalse;
					else
						*p = colorFont;
					++p;
				}
				row += pitch;
			}
			texture->unlock();
		}//end read32
		
		const s32 CTextureFont::getWidthFromCharacter(const wchar_t c)
		{
			u32 n = c - 32;
			
			if (n > m_PositionData.size())
				n = m_WrongChar;

			return m_PositionData[n].getWidth();
		}//end get w from chr
		
		const bool CTextureFont::loadTexture(io::IReadFile* file)
		{
			if(!m_ActiveDriver)
				return false;
			
			return CTextureFont::loadTexture(m_ActiveDriver->getTexture(file));
		}//end load texture IRF
		
		const bool CTextureFont::loadTexture(video::ITexture* texture)
		{
			if (!texture)
				return false;

			m_TextureRef = texture;
			m_TextureRef->grab();

			s32 lowerRightP = 0;
			
			switch(texture->getColorFormat())
			{
				case video::ECF_A1R5G5B5:
						readPositions16bit(texture, lowerRightP);
					break;
				case video::ECF_A8R8G8B8:
						readPositions32bit(texture, lowerRightP);
					break;
				default:
					return false;
			}//switch
			
			if (m_PositionData.size() > 127)
				m_WrongChar = 127;

			return (!m_PositionData.empty() && lowerRightP);
		}//load texture ptr end
		
		//public funcs IMPL
		const s32 CTextureFont::getCharacterFromPos(const wchar_t* text,const s32 pixel_x)
		{
			s32 x = 0;
			s32 idx = 0;

			while (text[idx])
			{
				x += getWidthFromCharacter(text[idx]);

				if (x >= pixel_x)
					return idx;

				++idx;
			}

			return -1;
		}//end get chr from p
		
		const bool CTextureFont::load(const c8* filename)
		{
			if(!m_ActiveDriver)
				return false;
			
			return loadTexture(m_ActiveDriver->getTexture(filename));
		}//end load fn
		
		const bool CTextureFont::load(io::IReadFile* file)
		{
			if(!m_ActiveDriver)
				return false;
			
			return loadTexture(m_ActiveDriver->getTexture(file));
		}//end load IRF
		
		void CTextureFont::render(const wchar_t* text, const core::position2d<s32>& pos,const video::SColor& color,const bool hcenter,const bool vcenter, const core::rect<s32>* clip)
		{
			static const core::dimension2d<s32> dummy(32,32);
			render(text,core::rect<s32>(pos,dummy),color,hcenter,vcenter,clip);
		}//end render
		
		//Here is an optimized version of the render() func
		void CTextureFont::renderOptimized(const wchar_t* text,
										   const core::position2d<s32>& pos,
										   const video::SColor& color,
										   const u32 deltaTime,
										   const bool frameIdp)
		{		
			u32 u = 0;
			
			static core::rect<s32> dummyRect  = core::rect<s32>(0,0,32,32);
			
			dummyRect.UpperLeftCorner.X = pos.X;
			dummyRect.UpperLeftCorner.Y = pos.Y;
			
			if( (!frameIdp) || (m_RenderTimer >= 8) )
			{
				for( ;*text; ++text )
				{
					u = ( *text ) - 32;
					
					if ( u > m_PositionData.size())
						u = m_WrongChar;
						
					m_ActiveDriver->draw2DImage(m_TextureRef, 
											    dummyRect.UpperLeftCorner, 
												m_PositionData[u], 
												0,
												color, 
												true);
												
					dummyRect.UpperLeftCorner.X += m_PositionData[u].getWidth();
				}
				
				m_RenderTimer = 0;
				return;
			}
			m_RenderTimer += deltaTime;
		}
		
		void CTextureFont::render(const wchar_t* text, const core::rect<s32>& position,const video::SColor& color,const bool hcenter,const bool vcenter, const core::rect<s32>* clip)
		{
			if (!m_ActiveDriver)
				return;

			core::dimension2d<s32> textDimension;
			core::position2d<s32> offset = position.UpperLeftCorner;

			if (hcenter || vcenter)
			{
				textDimension = getDimension(text);

				if (hcenter)
					offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;

				if (vcenter)
					offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
			}

			u32 n = 0;
			
			while(*text)
			{
				n = (*text) - 32;
				
				if ( n > m_PositionData.size())
					n = m_WrongChar;
					
				m_ActiveDriver->draw2DImage(m_TextureRef, offset, m_PositionData[n], clip, color, true);
				offset.X += m_PositionData[n].getWidth();

				++text;
			}
		}//end render
		
		core::dimension2d<s32> CTextureFont::getDimension(const wchar_t* text)
		{
			core::dimension2d<s32> dim(0, m_PositionData[0].getHeight());

			u32 n = 0;

			for(const wchar_t* p = text; *p; ++p)
			{
				n = (*p) - 32;
				if ( n > m_PositionData.size())
					n = m_WrongChar;

				dim.Width += m_PositionData[n].getWidth();
			}

			return dim;
		}
		
		const s32  CTextureFont::getHeight() const
		{
			return m_PositionData[0].getHeight();
		}
		

	};
};