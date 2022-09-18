#include "./../include/CompletMeshRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Io.hpp"
#include "./../include/Utils.hpp"
#include "./../include/Viewer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <fstream>
using namespace std;


CompletMeshRenderable::CompletMeshRenderable(
    ShaderProgramPtr shaderProgram,
    const std::string& meshName,
    const std::string& meshComponentName,
    const std::string& texture_filename
    ) : 

    HierarchicalRenderable(shaderProgram),
    m_pBuffer(0), m_cBuffer(0), m_nBuffer(0), m_iBuffer(0), m_tBuffer(0), m_texId( 0 )
    
{
    read_obj("../../sfmlGraphicsPipeline/meshes/"+meshName+"/"+meshComponentName+".obj", m_positions, m_indices, m_normals, m_texCoords);
    m_colors.resize( m_positions.size() );

    if(true)
    {
        ifstream inFile;    
        inFile.open("../../sfmlGraphicsPipeline/meshes/"+meshName+"/"+meshComponentName+".txt"    
            );    
        if (!inFile) {    
            cerr << "Offset Unable to open file datafile.txt\n";    
            exit(1);   // call system to stop    
        }
    
        double x,y,z;    
        inFile >>x>>y>>z;
        glm::mat4 iniParentTransform;

        setLocalTransform(glm::translate(iniParentTransform,glm::vec3(-x,-y,-z)));
        //setInitialParentTransform(glm::translate(iniParentTransform,glm::vec3(-x,-y,-z)));
        //setParentTransform(glm::translate(iniParentTransform,glm::vec3(-x,-y,-z)));
        inFile.close();        
    }
    
    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_cBuffer); //colors
    glGenBuffers(1, &m_nBuffer); //normals
    glGenBuffers(1, &m_iBuffer); //indices
    glcheck(glGenBuffers(1, &m_tBuffer)); //texture coordinates


    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*sizeof(glm::vec2), m_texCoords.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer));
    glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW));

    

    // create and setup the texture
    glcheck(glGenTextures(1, &m_texId));
    glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // load the texture
    sf::Image image;
    image.loadFromFile("./../../sfmlGraphicsPipeline/textures/"+texture_filename);
    image.flipVertically(); // sfml inverts the v axis... put the image in OpenGL convention: lower left corner is (0,0)
    glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)image.getPixelsPtr()));

    //Release the texture
    glcheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void CompletMeshRenderable::setUpAnimation(  const std::string& meshName,const std::string& meshComponentName, Viewer& viewer){ // lit le fichie d'animation et Set les animations
    ifstream inFile;
    string path = "../../sfmlGraphicsPipeline/meshes/"+meshName+"/animation/"+meshComponentName+"_keyframes.txt";
    inFile.open(path);
    if (!inFile) {
        cerr << "Animation Unable to open file datafile.txt\n";
        cerr << "path" << path << "\n";
        exit(1);   // call system to stop
    }

    double dureeAnimation;
    inFile>>dureeAnimation;

    double second;
    double xTrans , yTrans , zTrans, xRot , yRot , zRot, xScale , yScale, zScale;


    viewer.setAnimationLoop(true, dureeAnimation);
    //addParentTransformKeyframe(GeometricTransformation(),0);

    

    while(!inFile.eof()) {
        inFile >> second >> xTrans >> yTrans >> zTrans;
        inFile >> xRot >> yRot >> zRot;
        inFile >> xScale >> yScale >> zScale;
        addParentTransformKeyframe(GeometricTransformation(
             glm::vec3(xTrans,yTrans,zTrans),
             glm::quat(glm::vec3(xRot,yRot,zRot)),
             glm::vec3(xScale,yScale,zScale)),second);
    }

    inFile.close();
    viewer.startAnimation();
}

void CompletMeshRenderable::do_draw()
{
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int texcoordLocation = m_shaderProgram->getAttributeLocation("vTexCoord");
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
    int nitLocation = m_shaderProgram->getUniformLocation("NIT");
    int texsamplerLocation = m_shaderProgram->getUniformLocation("texSampler");

    //Send material uniform to GPU
    Material::sendToGPU(m_shaderProgram, m_material);


    if(modelLocation != ShaderProgram::null_location)
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(positionLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(colorLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
        glcheck(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if( nitLocation != ShaderProgram::null_location )
      {
        glcheck(glUniformMatrix3fv( nitLocation, 1, GL_FALSE,
          glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(getModelMatrix()))))));
      }

    //Bind texture in Textured Unit 0
    if(texcoordLocation != ShaderProgram::null_location)
    {
        glcheck(glActiveTexture(GL_TEXTURE0));
        glcheck(glBindTexture(GL_TEXTURE_2D, m_texId));
        //Send "texSampler" to Textured Unit 0
        glcheck(glUniform1i(texsamplerLocation, 0));
        glcheck(glEnableVertexAttribArray(texcoordLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_tBuffer));
        glcheck(glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }



    //Draw triangles elements
    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer));
    glcheck(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0));

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(colorLocation));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }

    if(texcoordLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(texcoordLocation ));
    }
}

void CompletMeshRenderable::setInitialLocalTransform( const glm::mat4& parentTransform ){
    initial_local_transform=parentTransform;
    setLocalTransform(parentTransform);
}

void CompletMeshRenderable::setInitialParentTransform( const glm::mat4& parentTransform ){
    initial_parent_transform=parentTransform;
    setParentTransform(parentTransform);
}

void CompletMeshRenderable::addLocalTransformKeyframe( const GeometricTransformation& transformation, float time )
{
  m_localKeyframes.add( transformation, time );
}

void CompletMeshRenderable::addParentTransformKeyframe( const GeometricTransformation& transformation, float time )
{
  m_parentKeyframes.add( transformation, time );
}

void CompletMeshRenderable::do_animate( float time )
{
    //Assign the interpolated transformations from the keyframes to the local/parent transformations.
    if(!m_localKeyframes.empty())
    {
        setLocalTransform(initial_local_transform * m_localKeyframes.interpolateTransformation( time ) );
    }
    if(!m_parentKeyframes.empty())
    {
        setParentTransform( initial_parent_transform * m_parentKeyframes.interpolateTransformation( time ) );
    }
}

CompletMeshRenderable::~CompletMeshRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
    glcheck(glDeleteBuffers(1, &m_iBuffer));
    glcheck(glDeleteBuffers(1, &m_tBuffer));
    glcheck(glDeleteTextures(1, &m_texId));
}




void CompletMeshRenderable::setMaterial(const MaterialPtr& material)
{
    m_material = material;
}
