#include "common/file/format_convert.h"





void FileConvert() {

	//vtk_to_off("C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308.1.vtk", "C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308_vtk.off");
	//vtk_to_obj("C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308.1.vtk", "C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308_vtk.obj");


	vtk_to_f3grid("C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308.1.vtk", "C:/Users/xmyci/Desktop/tetgen1.6.0/build/23308_vtk.f3grid");
	//vtk_to_obj("C:/Users/xmyci/Desktop/tetgen1.6.0/build/yulongshan_fix.1.vtk", "C:/Users/xmyci/Desktop/tetgen1.6.0/build/yulongshan_fix_vtk.obj");
	//vtk_to_off("C:/Users/xmyci/Desktop/tetgen1.6.0/build/yulongshan_fix.1.vtk", "C:/Users/xmyci/Desktop/tetgen1.6.0/build/yulongshan_fix_vtk.off");
}