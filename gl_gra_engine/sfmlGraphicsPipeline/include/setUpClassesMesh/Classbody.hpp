
	#ifndef HPP_body
	#define HPP_body

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Classbody {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			

		auto body = std::make_shared<CompletMeshRenderable>(shader,"body","body","UVbody.png");
			

		

		auto jAD = std::make_shared<CompletMeshRenderable>(shader,"body","jAD","UVjAD.png");
			

		

		

		auto aileG = std::make_shared<CompletMeshRenderable>(shader,"body","aileG","UVaileG.png");
			

		

		

		auto jDG = std::make_shared<CompletMeshRenderable>(shader,"body","jDG","UVjDG.png");
			

		

		

		auto queue = std::make_shared<CompletMeshRenderable>(shader,"body","queue","UVqueue.png");
			

		

		

		auto jAG = std::make_shared<CompletMeshRenderable>(shader,"body","jAG","UVjAG.png");
			

		

		

		auto JDD = std::make_shared<CompletMeshRenderable>(shader,"body","JDD","UVJDD.png");
			

		

		

		auto aileD = std::make_shared<CompletMeshRenderable>(shader,"body","aileD","UVaileD.png");
			

		

		

		
			
			HierarchicalRenderable::addChild( body, jAD);
			HierarchicalRenderable::addChild( body, aileG);
			HierarchicalRenderable::addChild( body, jDG);
			HierarchicalRenderable::addChild( body, queue);
			HierarchicalRenderable::addChild( body, jAG);
			HierarchicalRenderable::addChild( body, JDD);
			HierarchicalRenderable::addChild( body, aileD);
			

			viewer.addRenderable(body);
			
			
			jAD->setUpAnimation("body","jAD",viewer);
			aileG->setUpAnimation("body","aileG",viewer);
			jDG->setUpAnimation("body","jDG",viewer);
			body->setUpAnimation("body","body",viewer);
			queue->setUpAnimation("body","queue",viewer);
			jAG->setUpAnimation("body","jAG",viewer);
			JDD->setUpAnimation("body","JDD",viewer);
			aileD->setUpAnimation("body","aileD",viewer);
			
		}
	};

	#endif
	