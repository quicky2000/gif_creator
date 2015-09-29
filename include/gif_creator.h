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
    inline gif_creator(const std::string & p_file_name,
		       const unsigned int & p_delay_ms,
		       const unsigned int & p_x_origin,
		       const unsigned int & p_y_origin,
		       const unsigned int & p_width,
		       const unsigned int & p_height,
		       const unsigned int & p_start_index,
		       const unsigned int & p_nb_files);
    inline ~gif_creator(void);
  private:
    static inline const unsigned int & convert_to_gif_color(const lib_bmp::my_color_alpha & p_bmp_color,
							    const std::map<lib_bmp::my_color_alpha,unsigned int> & p_conversion_table);
    static inline void truncate_file_name(const std::string & p_name,
					  std::string & p_result);

    std::vector<std::pair<std::string,lib_bmp::my_bmp *> > m_bmp_files;
  };



  //----------------------------------------------------------------------------
  gif_creator::gif_creator(const std::string & p_file_name,
			   const unsigned int & p_delay_ms,
			   const unsigned int & p_x_origin,
			   const unsigned int & p_y_origin,
			   const unsigned int & p_width,
			   const unsigned int & p_height,
			   const unsigned int & p_start_index,
			   const unsigned int & p_nb_files)
  {
    size_t l_pattern_pos = p_file_name.find("#");
    if(std::string::npos == l_pattern_pos) throw quicky_exception::quicky_logic_exception("Failed to find '#' character in file patter \"" + p_file_name + "\"",__LINE__,__FILE__);

    // Search files matching pattern
    std::vector<std::string> l_file_list;
    bool l_continu = true;
    unsigned int l_file_index = p_start_index;
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
	if(p_nb_files == l_file_list.size())
	  {
	    l_continu = false;
	  }
      }
    while(l_continu);

    if(!l_file_list.size())
      {
	throw quicky_exception::quicky_logic_exception("No file found matching pattern \"" + p_file_name + "\"",__LINE__,__FILE__);
      }

    // Iterator on files to insure they have coherent dimensions, palette size and store them in memory
    std::set<lib_bmp::my_color_alpha> l_unified_bmp_palette;
    unsigned int l_file_width = 0;
    unsigned int l_file_height = 0;
    for(auto l_file_iter : l_file_list)
      {
	std::cout << "Load file \"" << l_file_iter << "\"" << std::endl ;
	// Load BMP files
	lib_bmp::my_bmp *l_bmp = new lib_bmp::my_bmp(l_file_iter);
	m_bmp_files.push_back(std::pair<std::string,lib_bmp::my_bmp *>(l_file_iter,l_bmp));

	// Store Picuture dimensions
	if(!l_file_width && ! l_file_height)
	  {
	    l_file_width = l_bmp->get_width();
	    l_file_height = l_bmp->get_height();
	    if(p_x_origin + p_width > l_file_width)
	      {
		throw quicky_exception::quicky_logic_exception("Width indicated in parameter with Origin x is greater than file width",__LINE__,__FILE__);
	      }
	    if(p_y_origin + p_height > l_file_height)
	      {
		throw quicky_exception::quicky_logic_exception("Width indicated in parameter with Origin x is greater than file width",__LINE__,__FILE__);
	      }
	  }
	else if(l_file_width != l_bmp->get_width() || l_file_height != l_bmp->get_height())
	  {
	    std::stringstream l_reference_width;
	    l_reference_width << l_file_width;
	    std::stringstream l_reference_height;
	    l_reference_height << l_file_height;
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

    unsigned int l_output_width = p_width ? p_width : l_file_width;
    unsigned int l_output_height = p_height ? p_height : l_file_height;
    unsigned int l_max_x = p_x_origin + l_output_width;
    unsigned int l_max_y = p_y_origin + l_output_height;
    lib_gif::gif l_gif(l_output_width,l_output_height);

    // generate a comment to store parameters
    std::string l_comment_file_name;
    truncate_file_name(p_file_name,l_comment_file_name);

    std::stringstream l_comment_stream;
    l_comment_stream << "Generated by gif_creator.exe with parameters : " << l_comment_file_name << " --delay-ms=" << p_delay_ms << " --x-origin=" << p_x_origin << " --y-origin=" << p_y_origin << " --width=" << p_width << " --height=" << p_height << " --start-index=" << p_start_index << " --nb-files=" << p_nb_files ;

    l_gif.add_comment(l_comment_stream.str());

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
    truncate_file_name(m_bmp_files.front().first,l_comment_file_name);
    l_gif.add_comment(l_comment_file_name);

    // Directly Convert first BMP file into a GIF picture
    lib_bmp::my_bmp *l_bmp = m_bmp_files.front().second;
    lib_gif::gif_image * l_gif_image = new lib_gif::gif_image(l_output_width,l_output_height);
    for(unsigned int l_y = p_y_origin ; l_y < l_max_y ; ++l_y)
      {
	for(unsigned int l_x = p_x_origin ; l_x < l_max_x ; ++l_x)
	  {
	    l_gif_image->set_color_index(l_x - p_x_origin,l_y - p_y_origin,convert_to_gif_color(l_bmp->get_pixel_color(l_x,l_y),l_conversion_colors));
	  }
      }
    l_gif.add_graphic_control_extension(p_delay_ms,false);
    l_gif.add_image(*l_gif_image);

    lib_bmp::my_bmp * l_previous_bmp_file = l_bmp;
    // For other files convert only differences to a GIF picture
    for(unsigned int l_bmp_index = 1 ; l_bmp_index < m_bmp_files.size() ; ++l_bmp_index)
      {
	std::cout << "Converting file \"" << m_bmp_files[l_bmp_index].first << std::endl ;
	truncate_file_name(m_bmp_files[l_bmp_index].first,l_comment_file_name);
	l_gif.add_comment(l_comment_file_name);

	l_bmp = m_bmp_files[l_bmp_index].second;

	// Compute differences with previous picture
	std::set<unsigned int> l_color_difference_index;
	std::map<std::pair<unsigned int,unsigned int>, unsigned int> m_differences;
	unsigned int l_min_x_diff = l_file_width;
	unsigned int l_max_x_diff = 0;
	unsigned int l_min_y_diff = l_file_height;
	unsigned int l_max_y_diff = 0;
	for(unsigned int l_y = p_y_origin ; l_y < l_max_y ; ++l_y)
	  {
	    for(unsigned int l_x = p_x_origin ; l_x < l_max_x ; ++l_x)
	      {
		const lib_bmp::my_color_alpha & l_bmp_color = l_bmp->get_pixel_color(l_x,l_y);
		const lib_bmp::my_color_alpha & l_previous_bmp_color = l_previous_bmp_file->get_pixel_color(l_x,l_y);
		// Check if pixel has changed
		if(l_previous_bmp_color != l_bmp_color)
		  {
		    // Store coordinates if they are extrema
		    if(l_min_x_diff > l_x)
		      {
			l_min_x_diff = l_x;
		      }
		    if(l_min_y_diff > l_y)
		      {
			l_min_y_diff = l_y;
		      }
		    if(l_max_x_diff < l_x)
		      {
			l_max_x_diff = l_x;
		      }
		    if(l_max_y_diff < l_y)
		      {
			l_max_y_diff = l_y;
		      }
		    unsigned int l_gif_color_index = convert_to_gif_color(l_bmp_color,l_conversion_colors);
		    l_color_difference_index.insert(l_gif_color_index);
		    m_differences.insert(std::map<std::pair<unsigned int,unsigned int>, unsigned int>::value_type(std::pair<unsigned int,unsigned int>(l_x,l_y),l_gif_color_index));
		  }
	      }
	  }

	// Compute transparent index
	unsigned int l_transparent_index = 0;
	while(l_transparent_index < 256 && l_color_difference_index.end() != l_color_difference_index.find(l_transparent_index))
	  {
	    ++l_transparent_index;
	  }
	assert(l_transparent_index < 256);
	std::cout << "Transparent index : " << l_transparent_index << "\tDiff window (" << l_min_x_diff << "," << l_min_y_diff << ")-(" << l_max_x_diff << "," << l_max_y_diff << ")" << std::endl ;

	l_gif.add_graphic_control_extension(p_delay_ms,true,l_transparent_index);

	unsigned int l_diff_width = l_max_x_diff - l_min_x_diff + 1;
	unsigned int l_diff_height = l_max_y_diff - l_min_y_diff + 1;
	l_gif_image = new lib_gif::gif_image(l_diff_width,l_diff_height,l_min_x_diff - p_x_origin,l_min_y_diff - p_y_origin);

	// Generate differential picture
	for(unsigned int l_y = 0 ; l_y < l_diff_height ; ++l_y)
	  {
	    for(unsigned int l_x = 0 ; l_x < l_diff_width ; ++l_x)
	      {
		std::map<std::pair<unsigned int,unsigned int>, unsigned int>::const_iterator l_pixel_iter = m_differences.find(std::pair<unsigned int,unsigned int>(l_x + l_min_x_diff,l_y + l_min_y_diff));
		l_gif_image->set_color_index(l_x,l_y,m_differences.end() == l_pixel_iter ? l_transparent_index : l_pixel_iter->second);
	      }
	  }
	l_gif.add_image(*l_gif_image);
	l_previous_bmp_file = l_bmp;
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
	delete l_iter.second;
      }
  }

  //----------------------------------------------------------------------------
  const unsigned int & gif_creator::convert_to_gif_color(const lib_bmp::my_color_alpha & p_bmp_color,
							 const std::map<lib_bmp::my_color_alpha,unsigned int> & p_conversion_table)
  {
    std::map<lib_bmp::my_color_alpha,unsigned int>::const_iterator l_color_iter = p_conversion_table.find(p_bmp_color);
    if(p_conversion_table.end() != l_color_iter)
      {
	return l_color_iter->second;
      }
    std::stringstream l_color_stream;
    l_color_stream << p_bmp_color;
    throw quicky_exception::quicky_logic_exception("BMP color \"" + l_color_stream.str() + " not found insigned unified GIF palette",__LINE__,__FILE__);
  }

  //----------------------------------------------------------------------------
  void gif_creator::truncate_file_name(const std::string & p_name,
				       std::string & p_result)
  {
    p_result = p_name;
    size_t l_pos = p_result.find_last_of("/");
    if(std::string::npos != l_pos && l_pos < p_result.size())
      {
	p_result = p_result.substr(l_pos + 1);
      }
    l_pos = p_result.find_last_of("\\");
    if(std::string::npos != l_pos && l_pos < p_result.size())
      {
	p_result = p_result.substr(l_pos + 1);
      }
  }

}
#endif // GIF_CREATOR_H
//EOF

