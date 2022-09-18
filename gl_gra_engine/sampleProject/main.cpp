#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedCubeRenderable.hpp>
#include <texturing/MultiTexturedCubeRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/BillBoardPlaneRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <texturing/TexturedTriangleRenderable.hpp>

#include <texturing/TexturedLightedMeshRenderable.hpp>


#include <KeyFramedHierarchicalMeshRenderable.hpp>
#include <CompletMeshRenderable.hpp>

#include <FrameRenderable.hpp>


#include <iostream>

//import des meshs
#include <setUpClassesMesh/Classbody.hpp>
#include <setUpClassesMesh/Classbois.hpp>
#include <setUpClassesMesh/Classterrain.hpp>
#include <setUpClassesMesh/Classpanier1.hpp>
#include <setUpClassesMesh/Classpanier2.hpp>
#include <setUpClassesMesh/Classball.hpp>

//import de dragon
//#include <setUpClassesMesh/Classdragon_body.hpp>


void initialize_scene( Viewer& viewer )
{

    //Textured shader
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );

    //Default shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);
   
    
    if(true){
        std::cout<<" Animation basket"<<std::endl;

        Classbody body;
        body.executeImport(viewer,texShader);
       /* Classakko akko;
        akko.executeImport(viewer,texShader);*/
        /*Classakko_jambe_droite jd;
        jd.executeImport(viewer,texShader);
        Classakko_jambe_gauche jg;
        jg.executeImport(viewer,texShader);
        Classbras_d bd;
        bd.executeImport(viewer,texShader);
        Classbras_g bg;
        bg.executeImport(viewer,texShader);*/
        Classbois bois;
        bois.executeImport(viewer,texShader);
        Classpanier1 panier1;
        panier1.executeImport(viewer,texShader);
        Classpanier2 panier2;
        panier2.executeImport(viewer,texShader);
        Classterrain terrain;
        terrain.executeImport(viewer,texShader);
        Classball basked;
        basked.executeImport(viewer,texShader);
    }else{
        std::cout<<" Animation dragon seulement"<<std::endl;
        
       /* Classdragon_body dragon;
        dragon.executeImport(viewer, texShader);*/
    }

    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, 0, 8 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );

    //Add a 3D frame to the viewer
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);
}

int main() 
{
	Viewer viewer(1280,720);
	initialize_scene(viewer);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	

	return EXIT_SUCCESS;
}
