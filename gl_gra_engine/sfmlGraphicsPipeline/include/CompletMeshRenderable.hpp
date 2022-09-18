#ifndef COMPLET_MESH_RENDERABLE_HPP
#define COMPLET_MESH_RENDERABLE_HPP

#include "HierarchicalRenderable.hpp"
#include "KeyframeCollection.hpp"

#include "./HierarchicalRenderable.hpp"
#include "./lighting/Material.hpp"
#include "./lighting/Light.hpp"


#include <string>
#include <vector>
# include <glm/glm.hpp>
# include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

class CompletMeshRenderable : public HierarchicalRenderable
{
    public:
        ~CompletMeshRenderable();
        CompletMeshRenderable(
            ShaderProgramPtr program,
            const std::string& meshName,
            const std::string& meshComponentName,
            const std::string& texture_filename
            );
        
        void addLocalTransformKeyframe( const GeometricTransformation& transformation, float time );
        void addParentTransformKeyframe( const GeometricTransformation& transformation, float time );

        virtual void setInitialParentTransform( const glm::mat4& parentTransform );
        virtual void setInitialLocalTransform( const glm::mat4& parentTransform );
        virtual void setUpAnimation(  const std::string& meshName,const std::string& meshComponentName, Viewer& viewer);

        
        void setMaterial(const MaterialPtr& material);


        
            virtual void do_draw();
            virtual void do_animate( float time );

            std::vector< glm::vec3 > m_positions;
            std::vector< glm::vec3 > m_normals;
            std::vector< glm::vec4 > m_colors;
        std::vector< glm::vec2 > m_texCoords;
            std::vector< unsigned int > m_indices;

            glm::mat4 initial_parent_transform;
            glm::mat4 initial_local_transform;

            
        KeyframeCollection m_localKeyframes; 
        KeyframeCollection m_parentKeyframes;

            unsigned int m_pBuffer;
            unsigned int m_cBuffer;
            unsigned int m_nBuffer;
            unsigned int m_iBuffer;
        unsigned int m_tBuffer;
        unsigned int m_texId;

        MaterialPtr m_material;

};

typedef std::shared_ptr<CompletMeshRenderable> CompletMeshRenderablePtr;

#endif
