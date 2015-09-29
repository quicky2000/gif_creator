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

#include "parameter_manager.h"
#include "gif_creator.h"


int main(int argc,char ** argv)
{

  try
    {
      // Defining application command line parameters
      parameter_manager::parameter_manager l_param_manager("gif_creator.exe","--",1);
      parameter_manager::parameter_if l_file_name_parameter("file-name",false);
      l_param_manager.add(l_file_name_parameter);
      parameter_manager::parameter_if l_delay_ms_parameter("delay-ms",true);
      l_param_manager.add(l_delay_ms_parameter);
      parameter_manager::parameter_if l_x_origin_parameter("x-origin",true);
      l_param_manager.add(l_x_origin_parameter);
      parameter_manager::parameter_if l_y_origin_parameter("y-origin",true);
      l_param_manager.add(l_y_origin_parameter);
      parameter_manager::parameter_if l_width_parameter("width",true);
      l_param_manager.add(l_width_parameter);
      parameter_manager::parameter_if l_height_parameter("height",true);
      l_param_manager.add(l_height_parameter);
      parameter_manager::parameter_if l_start_index_parameter("start-index",true);
      l_param_manager.add(l_start_index_parameter);
      parameter_manager::parameter_if l_nb_files_parameter("nb-files",true);
      l_param_manager.add(l_nb_files_parameter);
  
      // Treating parameters
      l_param_manager.treat_parameters(argc,argv);

      std::string l_file_name = l_file_name_parameter.get_value<std::string>();
      unsigned int l_delay = l_delay_ms_parameter.value_set() ? atoi(l_delay_ms_parameter.get_value<std::string>().c_str()) : 500;
      unsigned int l_x_origin = l_x_origin_parameter.value_set() ? atoi(l_x_origin_parameter.get_value<std::string>().c_str()) : 0;
      unsigned int l_y_origin = l_y_origin_parameter.value_set() ? atoi(l_y_origin_parameter.get_value<std::string>().c_str()) : 0;
      unsigned int l_width = l_width_parameter.value_set() ? atoi(l_width_parameter.get_value<std::string>().c_str()) : 0;
      unsigned int l_height = l_height_parameter.value_set() ? atoi(l_height_parameter.get_value<std::string>().c_str()) : 0;
      unsigned int l_start_index = l_start_index_parameter.value_set() ? atoi(l_start_index_parameter.get_value<std::string>().c_str()) : 0;
      unsigned int l_nb_files = l_nb_files_parameter.value_set() ? atoi(l_nb_files_parameter.get_value<std::string>().c_str()) : 0;
      gif_creator::gif_creator l_gif_creator(l_file_name,l_delay,l_x_origin,l_y_origin,l_width,l_height,l_start_index,l_nb_files);
    }
  catch(quicky_exception::quicky_runtime_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  catch(quicky_exception::quicky_logic_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  return 0;
}
//EOF
