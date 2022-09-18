
	#ifndef HPP_bois
	#define HPP_bois

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classbois {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto bois = std::make_shared<CompletMeshRenderable>(shader,"bois","bois","UVbois.png");
			

		

		
			
			

			viewer.addRenderable(bois);
			
			
			bois->setUpAnimation("bois","bois",viewer);
			
		}
	};

	#endif
	