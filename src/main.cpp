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
      parameter_manager::parameter_if l_file_name_parameter("file_name",false);
      l_param_manager.add(l_file_name_parameter);
  
      // Treating parameters
      l_param_manager.treat_parameters(argc,argv);

      std::string l_file_name = l_file_name_parameter.get_value<std::string>();

      gif_creator::gif_creator l_gif_creator(l_file_name);
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
