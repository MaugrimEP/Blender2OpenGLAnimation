
	#ifndef HPP_ball
	#define HPP_ball

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classball {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto ball = std::make_shared<CompletMeshRenderable>(shader,"ball","ball","UVball.png");
			

		

		
			
			

			viewer.addRenderable(ball);
			
			
			ball->setUpAnimation("ball","ball",viewer);
			
		}
	};

	#endif
	