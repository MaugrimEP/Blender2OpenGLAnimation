
	#ifndef HPP_terrain
	#define HPP_terrain

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classterrain {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto terrain = std::make_shared<CompletMeshRenderable>(shader,"terrain","terrain","UVterrain.png");
			

		

		
			
			

			viewer.addRenderable(terrain);
			
			
			terrain->setUpAnimation("terrain","terrain",viewer);
			
		}
	};

	#endif
	