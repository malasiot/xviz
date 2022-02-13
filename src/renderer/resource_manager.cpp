#include "resource_manager.hpp"

#include "shaders/common.vs.hpp"
#include "shaders/phong.fs.hpp"
#include "shaders/shadows.fs.hpp"
#include "shaders/constant.fs.hpp"
#include "shaders/per_vertex.fs.hpp"
#include "shaders/wireframe.hpp"

#include "shaders/shadow_map.vs.hpp"
#include "shaders/shadow_map.fs.hpp"
#include "shaders/lights.hpp"


#include <fstream>

namespace xviz { namespace impl {

std::string OpenGLShaderResourceManager::fetch(const std::string &name) {
    if ( name.empty() ) return {} ;

    if ( name[0] == '@' ) {
        auto it = instance_.sources_.find(name.substr(1)) ;
        if ( it != instance_.sources_.end() ) return it->second.c_str() ;
        else return {} ;
    } else {
        std::ifstream fs(instance_.folder_ + '/' + name);
        return {std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()};
    }
}

void OpenGLShaderResourceManager::setShaderResourceFolder(const std::string &folder) {
    instance_.folder_ = folder ;
}

OpenGLShaderResourceManager::OpenGLShaderResourceManager() {
    addSource("vertex_shader", vertex_shader_code) ;
    addSource("phong_fragment_shader_vars", phong_fragment_shader_vars) ;
    addSource("phong_fragment_shader_common", phong_fragment_shader_common) ;
    addSource("phong_fragment_shader", phong_fragment_shader) ;
    addSource("shadows_fragment_shader", shadows_fragment_shader);
    addSource("constant_fragment_shader_vars", constant_fragment_shader_vars);
    addSource("constant_fragment_shader", constant_fragment_shader) ;
    addSource("per_vertex_color_fragment_shader", per_vertex_color_fragment_shader);
    addSource("shadow_map_shader_vs", shadow_map_shader_vs) ;
    addSource("shadow_map_shader_fs", shadow_map_shader_fs) ;
    addSource("shadow_debug_shader_vs", shadow_debug_shader_vs) ;
    addSource("shadow_debug_shader_fs", shadow_debug_shader_fs) ;
    addSource("wireframe_fragment_shader", wireframe_shader_fs) ;
    addSource("wireframe_geometry_shader", wireframe_shader_gs) ;
    addSource("light_vars", light_vars) ;
}

void OpenGLShaderResourceManager::addSource(const char * name, const char *src) {
    sources_.emplace(name, src) ;
}

OpenGLShaderResourceManager OpenGLShaderResourceManager::instance_ ;



}}
