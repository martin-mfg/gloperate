/******************************************************************************\
 * gloperate
 *
 * Copyright (C) 2014 Computer Graphics Systems Group at the 
 * Hasso-Plattner-Institut (HPI), Potsdam, Germany.
\******************************************************************************/
#include <gloperate-qt/QtTextureStorer.h>
#include <gloperate-qt/qt-includes-begin.h>
#include <QString>
#include <QImage>
#include <QImageWriter>
#include <gloperate-qt/qt-includes-end.h>
#include <glbinding/gl/gl.h>
#include <globjects/Texture.h>


namespace gloperate_qt
{


/**
*  @brief
*    Constructor
*/
QtTextureStorer::QtTextureStorer()
: TextureStorer()
{
    // Get list of supported file formats
    QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    for (int i = 0; i < formats.size(); ++i) {
        std::string format = std::string(formats[i].data());
        m_extensions.push_back(std::string(".") + format);
        m_types.push_back(format + " image (*." + format + ")");
    }

    // Add entry that contains all supported file formats
    std::string allTypes;
    for (unsigned int i = 0; i < m_extensions.size(); ++i) {
        if (i > 0) allTypes += " ";
        allTypes += "*." + m_extensions[i].substr(1);
    }
    m_types.push_back(std::string("Qt image formats (") + allTypes + ")");
}

/**
*  @brief
*    Destructor
*/
QtTextureStorer::~QtTextureStorer()
{
}

/**
*  @brief
*    Check if this storer can store into a specific file type
*/
bool QtTextureStorer::canStore(const std::string & ext) const
{
     // Check if file type is supported
    return (std::count(m_extensions.begin(), m_extensions.end(), "." + ext) > 0);
}

/**
*  @brief
*    Get list of file types for storing
*/
std::vector<std::string> QtTextureStorer::storingTypes() const
{
    // Return list of supported file types
    return m_types;
}

/**
*  @brief
*    Get all file types for storing
*/
std::string QtTextureStorer::allStoringTypes() const
{
    // Compose list of all supported file extensions
    std::string allTypes;
    for (unsigned int i = 0; i < m_extensions.size(); ++i) {
        if (i > 0) allTypes += " ";
        allTypes += "*." + m_extensions[i].substr(1);
    }

    // Return supported types
    return allTypes;
}

/**
*  @brief
*    Store data to a file
*/
bool QtTextureStorer::store(const std::string & filename, const globjects::Referenced * object) const
{
    const globjects::Texture * texture = dynamic_cast<const globjects::Texture *>(object);

    if (!texture)
    {
        return false;
    }

    // Load image

    texture->bind();
    int width = texture->getLevelParameter(0, gl::GL_TEXTURE_WIDTH);
    int height = texture->getLevelParameter(0, gl::GL_TEXTURE_HEIGHT);

    if (width <= 0 || height <= 0)
    {
        return false;
    }

    QImage image(width, height, QImage::Format_RGB888);

    texture->bind();
    gl::glGetTexImage(texture->target(), 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, image.bits());

    return image.mirrored().save(QString::fromStdString(filename));
}


} // namespace gloperate_qt