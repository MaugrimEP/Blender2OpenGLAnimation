
	#ifndef HPP_panier2
	#define HPP_panier2

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classpanier2 {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto panier2 = std::make_shared<CompletMeshRenderable>(shader,"panier2","panier2","UVpanier2.png");
			

		

		
			
			

			viewer.addRenderable(panier2);
			
			
			panier2->setUpAnimation("panier2","panier2",viewer);
			
		}
	};

	#endif
	