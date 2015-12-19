#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>

#include <glm/vec3.hpp>                 // glm::vec3
#include <glm/vec2.hpp>                 // glm::vec4
#include <glm/vec4.hpp>                 // glm::vec4
#include <glm/mat4x4.hpp>               // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective, glm::lookAtRH
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr
#include <glm/gtc/matrix_inverse.hpp>

//====================================================================================================================================
// DATA related

#define TP_PATH_TO_DATA "data/"

//====================================================================================================================================
// ERROR & DEBUG related

#define DEBUG

// DEBUG_BREAK
#ifdef  WIN32
#   define  TP_DEBUG_BREAK()    __debugbreak()
#else
#   define  TP_DEBUG_BREAK()    asm("int $3")
#endif


// LOG
#define TP_LOG( _message, ... )  fprintf( stdout, (_message), __VA_ARGS__ );fflush( stdout )

// ASSERT
#ifdef DEBUG
#   define TP_ASSERT(_assert, ...)   \
        if( !(_assert) ) {  \
            TP_LOG( "Assertion failed !\n.FILE = '%s'\n.LINE='%d'\n.FUNCTION='%s'\n", __FILE__ , __LINE__, __FUNCTION__);\
            TP_LOG( __VA_ARGS__ );\
            TP_DEBUG_BREAK();\
            assert(_assert); \
        }
#else
#   define TP_ASSERT(_assert, ...)
#endif


//====================================================================================================================================
std::string readFileSrc( const std::string& _rstrFilePath );

//====================================================================================================================================
bool loadOBJ(   const char* _strPath
            ,   std::vector< glm::vec3 >& _rOutVertices
            ,   std::vector< glm::vec2 >& _rOutUVs
            ,   std::vector< glm::vec3 >& _rOutNormals );

#endif // COMMON_H

