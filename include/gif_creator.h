/*    This file is part of gif_creator
      Copyright (C) 2015  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef GIF_CREATOR_H
#define GIF_CREATOR_H

#include "gif.h"
#include "gif_image.h"
#include "my_bmp.h"
#include "quicky_exception.h"
#include <string>
#include <fstream>
#include <sstream>

namespace gif_creator
{
  class gif_creator
  {
  public:
    inline gif_creator(const std::string & p_file_name);
  private:
  };



  //----------------------------------------------------------------------------
  gif_creator::gif_creator(const std::string & p_file_name)
  {
    // Check if file exist
    std::ifstream l_file;
    l_file.open(p_file_name.c_str(),std::ifstream::binary);
    if(!l_file) throw quicky_exception::quicky_runtime_exception("Unable to read file \""+p_file_name+"\"",__LINE__,__FILE__);
    l_file.close();
      
    // Load BMP files
    lib_bmp::my_bmp l_bmp(p_file_name);
    l_bmp.display_info();

    std::cout << "Nb bits per pixel = " << l_bmp.get_nb_bits_per_pixel() << std::endl ;
    if( 8 != l_bmp.get_nb_bits_per_pixel())
      {
	std::stringstream l_stream ;
	l_stream << l_bmp.get_nb_bits_per_pixel();
	throw quicky_exception::quicky_logic_exception("Only BMP files with 8 bits per pixel are supported : "+l_stream.str(),__LINE__,__FILE__);
      }

    std::cout << "Image width : " << l_bmp.get_width() << std::endl ;
    std::cout << "Image height : " << l_bmp.get_height() << std::endl ;

    lib_gif::gif l_gif(l_bmp.get_width(),l_bmp.get_height());

    // Copy BMP file palette to GIF palette
    uint32_t l_palette_size = l_bmp.get_palette().get_size();
    for(unsigned int l_index = 0 ; l_index < l_palette_size ; ++l_index)
    {
      const lib_bmp::my_color_alpha & l_bmp_color = l_bmp.get_palette().get_color(l_index);
      lib_gif::gif_color l_gif_color(l_bmp_color.get_red(),l_bmp_color.get_green(),l_bmp_color.get_blue());
      l_gif.set_color_in_global_color_table(l_index,l_gif_color);
    }


    lib_gif::gif_image * l_gif_image = new lib_gif::gif_image(l_bmp.get_width(),l_bmp.get_height());
    for(unsigned int l_y = 0 ; l_y < l_bmp.get_height() ; ++l_y)
      {
        for(unsigned int l_x = 0 ; l_x < l_bmp.get_width() ; ++l_x)
          {
            l_gif_image->set_color_index(l_x,l_y,l_bmp.get_palette().get_index(l_bmp.get_pixel_color(l_x,l_y)));
          }
      }
    l_gif.add_image(*l_gif_image);
    std::cout << "GIF file : " << std::endl << l_gif ;

    // Determine output name
    size_t l_pos = p_file_name.rfind('.');
    if(std::string::npos == l_pos)
      {
	throw quicky_exception::quicky_logic_exception("Unable to find '.' character in file name \""+p_file_name+"\"",__LINE__,__FILE__);
      }
    std::string l_output_file_name = p_file_name.substr(0,l_pos) + ".gif";
    std::cout << "Output file name : \"" << l_output_file_name << "\"" << std::endl ;

    // Create output file
    std::ofstream l_output_file;
    l_output_file.open(l_output_file_name.c_str(),std::ofstream::binary);
    if(!l_output_file) throw quicky_exception::quicky_runtime_exception("Unable to create file \""+l_output_file_name+"\"",__LINE__,__FILE__);

    l_gif.write(l_output_file);
    l_output_file.close();

  }
}
#endif // GIF_CREATOR_H
//EOF

