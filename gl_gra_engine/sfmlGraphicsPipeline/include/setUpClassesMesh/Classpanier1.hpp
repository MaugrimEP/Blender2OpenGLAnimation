
	#ifndef HPP_panier1
	#define HPP_panier1

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classpanier1 {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto panier1 = std::make_shared<CompletMeshRenderable>(shader,"panier1","panier1","UVpanier1.png");
			

		

		
			
			

			viewer.addRenderable(panier1);
			
			
			panier1->setUpAnimation("panier1","panier1",viewer);
			
		}
	};

	#endif
	