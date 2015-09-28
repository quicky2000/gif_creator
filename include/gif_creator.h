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
#include <vector>
#include <set>

namespace gif_creator
{
  class gif_creator
  {
  public:
    inline gif_creator(const std::string & p_file_name);
    inline ~gif_creator(void);
  private:
    std::vector<lib_bmp::my_bmp *> m_bmp_files;
  };



  //----------------------------------------------------------------------------
  gif_creator::gif_creator(const std::string & p_file_name)
  {
    size_t l_pattern_pos = p_file_name.find("#");
    if(std::string::npos == l_pattern_pos) throw quicky_exception::quicky_logic_exception("Failed to find '#' character in file patter \"" + p_file_name + "\"",__LINE__,__FILE__);


    std::vector<std::string> l_file_list;
    bool l_continu = true;
    unsigned int l_file_index = 0;
    do
      {
	std::string l_file_name = p_file_name;
	std::stringstream l_file_index_stream;
	l_file_index_stream << l_file_index;
	l_file_name.replace(l_pattern_pos,1,l_file_index_stream.str());

	// Check if file exist
	std::ifstream l_file;
	l_file.open(l_file_name.c_str(),std::ifstream::binary);
	if(l_file.is_open())
	  {
	    l_file.close();
	    l_file_list.push_back(l_file_name);
	    std::cout << "Discover file \"" << l_file_name << "\"" << std::endl;
	    ++l_file_index;
	  }
	else
	  {
	    l_continu = false;
	  }
      }
    while(l_continu);

    std::set<lib_bmp::my_color_alpha> l_unified_bmp_palette;
    unsigned int l_width = 0;
    unsigned int l_height = 0;
    for(auto l_file_iter : l_file_list)
      {
	// Load BMP files
	lib_bmp::my_bmp *l_bmp = new lib_bmp::my_bmp(l_file_iter);
	m_bmp_files.push_back(l_bmp);

	// DIsplay BMP information
	//l_bmp->display_info();
	std::cout << "Image width : " << l_bmp->get_width() << std::endl ;
	std::cout << "Image height : " << l_bmp->get_height() << std::endl ;
	std::cout << "Nb bits per pixel = " << l_bmp->get_nb_bits_per_pixel() << std::endl ;

	if(!l_width && ! l_height)
	  {
	    l_width = l_bmp->get_width();
	    l_height = l_bmp->get_height();
	  }
	else if(l_width != l_bmp->get_width() || l_height != l_bmp->get_height())
	  {
	    std::stringstream l_reference_width;
	    l_reference_width << l_width;
	    std::stringstream l_reference_height;
	    l_reference_height << l_height;
	    std::stringstream l_new_width ;
	    l_new_width << l_bmp->get_width();
	    std::stringstream l_new_height ;
	    l_new_height << l_bmp->get_height();
	    throw quicky_exception::quicky_logic_exception("File \"" + l_file_iter + "\" has different dimensions (" + l_new_width.str() + "," + l_new_height.str() + ") compare to previous file dimensions("+l_reference_width.str()+","+l_reference_height.str()+")",__LINE__,__FILE__);
	  }
	// Check number of bits per pixel
	if( 8 != l_bmp->get_nb_bits_per_pixel())
	  {
	    std::stringstream l_stream ;
	    l_stream << l_bmp->get_nb_bits_per_pixel();
	    throw quicky_exception::quicky_logic_exception("Only BMP files with 8 bits per pixel are supported : "+l_stream.str(),__LINE__,__FILE__);
	  }

	// Fill the unified bmp Palette
	uint32_t l_palette_size = l_bmp->get_palette().get_size();
	for(unsigned int l_index = 0 ; l_index < l_palette_size ; ++l_index)
	  {
	    l_unified_bmp_palette.insert(l_bmp->get_palette().get_color(l_index));
	  }
      }

    std::cout << "Unified Palette size : " << l_unified_bmp_palette.size() << std::endl;
    if(l_unified_bmp_palette.size() > 256)
      {
	std::stringstream l_size_stream;
	l_size_stream << l_unified_bmp_palette.size();
	throw quicky_exception::quicky_logic_exception("Unified palette size is greater than 256 : " + l_size_stream.str(),__LINE__,__FILE__);
      }

    lib_gif::gif l_gif(l_width,l_height);

    // Copy BMP file palette to GIF palette
    unsigned int l_index = 0;
    std::map<lib_bmp::my_color_alpha,unsigned int> l_conversion_colors;
    for(auto l_color_iter : l_unified_bmp_palette)
      {
	lib_gif::gif_color l_gif_color(l_color_iter.get_red(),l_color_iter.get_green(),l_color_iter.get_blue());
	l_conversion_colors.insert(std::map<lib_bmp::my_color_alpha,unsigned int>::value_type(l_color_iter,l_index));
	l_gif.set_color_in_global_color_table(l_index,l_gif_color);
	++l_index;
      }

    l_gif.activate_loop(0);

    // Convert each BMP file into a GIF picture
    for(auto l_bmp_iter : m_bmp_files)
      {
	l_gif.add_graphic_control_extension(100,false);

	lib_gif::gif_image * l_gif_image = new lib_gif::gif_image(l_width,l_height);
	for(unsigned int l_y = 0 ; l_y < l_height ; ++l_y)
	  {
	    for(unsigned int l_x = 0 ; l_x < l_width ; ++l_x)
	      {
		const lib_bmp::my_color_alpha & l_bmp_color = l_bmp_iter->get_pixel_color(l_x,l_y);
		std::map<lib_bmp::my_color_alpha,unsigned int>::const_iterator l_color_iter = l_conversion_colors.find(l_bmp_color);
		if(l_conversion_colors.end() == l_color_iter)
		  {
		    std::stringstream l_color_stream;
		    l_color_stream << "R" << std::setw(3) << (uint32_t)l_bmp_color.get_red() << ", G=" << std::setw(3) << (uint32_t)l_bmp_color.get_green() << ", B=" << std::setw(3) << (uint32_t)l_bmp_color.get_blue();
		    throw quicky_exception::quicky_logic_exception("BMP color \"" + l_color_stream.str() + " not found insigned unified GIF palette",__LINE__,__FILE__);
		  }
		l_gif_image->set_color_index(l_x,l_y,l_color_iter->second);
	      }
	  }
	l_gif.add_image(*l_gif_image);
      }

    // Determine output name
    size_t l_pos = p_file_name.rfind('.');
    if(std::string::npos == l_pos)
      {
	throw quicky_exception::quicky_logic_exception("Unable to find '.' character in file name \""+p_file_name+"\"",__LINE__,__FILE__);
      }
    std::string l_output_file_name = p_file_name.substr(0,l_pos) + ".gif";
    l_pos = l_output_file_name.find("_#");
    if(std::string::npos != l_pos)
      {
	l_output_file_name.replace(l_pos,2,"");
      }
    l_pos = l_output_file_name.find("#");
    if(std::string::npos != l_pos)
      {
	l_output_file_name.replace(l_pos,1,"");
      }
    std::cout << "Output file name : \"" << l_output_file_name << "\"" << std::endl ;

    // Create output file
    std::ofstream l_output_file;
    l_output_file.open(l_output_file_name.c_str(),std::ofstream::binary);
    if(!l_output_file) throw quicky_exception::quicky_runtime_exception("Unable to create file \""+l_output_file_name+"\"",__LINE__,__FILE__);

    l_gif.write(l_output_file);
    l_output_file.close();
  }

  //----------------------------------------------------------------------------
  gif_creator::~gif_creator(void)
  {
    for(auto l_iter: m_bmp_files)
      {
	delete l_iter;
      }
  }
}
#endif // GIF_CREATOR_H
//EOF

