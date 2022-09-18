#include "./../include/KeyFramedHierarchicalMeshRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Io.hpp"
#include "./../include/Utils.hpp"
#include "./../include/Viewer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <fstream>
using namespace std;
KeyFramedHierarchicalMeshRenderable::KeyFramedHierarchicalMeshRenderable( ShaderProgramPtr shaderProgram, const std::string& meshName,const std::string& meshComponentName) : 
    HierarchicalRenderable(shaderProgram),
    m_pBuffer(0), m_cBuffer(0), m_nBuffer(0), m_iBuffer(0)
{
    std::vector<glm::vec2> texCoords;
    read_obj("../../sfmlGraphicsPipeline/meshes/"+meshName+"/"+meshComponentName+".obj", m_positions, m_indices, m_normals, texCoords);
    m_colors.resize( m_positions.size() );
    for(size_t i=0; i<m_colors.size(); ++i)
        m_colors[i] = randomColor();

    {
        ifstream inFile;    
        inFile.open("../../sfmlGraphicsPipeline/meshes/"+meshName+"/"+meshComponentName+".txt"    
            );    
        if (!inFile) {    
            cerr << "Unable to open file datafile.txt";    
            exit(1);   // call system to stop    
        }
    
        double x,y,z;    
        inFile >>x>>y>>z;
        glm::mat4 iniParentTransform;

        setLocalTransform(glm::translate(iniParentTransform,glm::vec3(-x,-y,-z)));
        inFile.close();        
    }
    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_cBuffer); //colors
    glGenBuffers(1, &m_nBuffer); //normals
    glGenBuffers(1, &m_iBuffer); //indices

    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer));
    glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW));
}

void KeyFramedHierarchicalMeshRenderable::setUpAnimation(  const std::string& meshName,const std::string& meshComponentName, Viewer& viewer){ // lit le fichie d'animation et Set les animations
    ifstream inFile;
    inFile.open("../../sfmlGraphicsPipeline/meshes/"+meshName+"/animation/"+meshComponentName+"_keyframes.txt");
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
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

void KeyFramedHierarchicalMeshRenderable::do_draw()
{
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");

    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");

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
}

void KeyFramedHierarchicalMeshRenderable::setInitialLocalTransform( const glm::mat4& parentTransform ){
    initial_local_transform=parentTransform;
    setLocalTransform(parentTransform);
}

void KeyFramedHierarchicalMeshRenderable::setInitialParentTransform( const glm::mat4& parentTransform ){
    initial_parent_transform=parentTransform;
    setParentTransform(parentTransform);
}

void KeyFramedHierarchicalMeshRenderable::addLocalTransformKeyframe( const GeometricTransformation& transformation, float time )
{
  m_localKeyframes.add( transformation, time );
}

void KeyFramedHierarchicalMeshRenderable::addParentTransformKeyframe( const GeometricTransformation& transformation, float time )
{
  m_parentKeyframes.add( transformation, time );
}

void KeyFramedHierarchicalMeshRenderable::do_animate( float time )
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

KeyFramedHierarchicalMeshRenderable::~KeyFramedHierarchicalMeshRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
    glcheck(glDeleteBuffers(1, &m_iBuffer));
}
