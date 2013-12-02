#include "positionable.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "../perfmon/profiler.h"

namespace ramen
{
	const uint32_t Positionable::id = 0x00000001;

	Positionable::Positionable(const bool isCamera)
		: m_dirty(true)
		, m_isCamera(isCamera)
		, m_scale(1.f)
	{
	}

	void Positionable::lookAt(const glm::vec3& at)
	{
		assert(at != m_translation);

		glm::vec3 direction = glm::normalize(at - m_translation);
		m_rotation.x = glm::degrees(glm::asin(-direction.y));
		m_rotation.y = -glm::degrees(glm::atan(-direction.x, -direction.z));

		m_dirty = true;
	}

	void Positionable::update()
	{
		if (m_dirty) {
			PROFILE;
			glm::quat orientation = glm::quat(glm::radians(m_rotation));

			m_matrix = glm::mat4x4(1.f);
			m_matrix = glm::scale(m_matrix, m_scale);
			m_matrix *= glm::mat4_cast(orientation);

			if (m_isCamera) {
				m_matrix = glm::translate(m_matrix, -m_translation);
			} else {
				m_matrix = glm::translate(m_matrix, m_translation);
			}

			m_dirty = false;
		}
	}

	void Positionable::offsetTranslation(const glm::vec3& offset)
	{
		m_translation += offset;
		m_dirty = true;
	}

	void Positionable::offsetRotation(const glm::vec3& offset)
	{
		m_rotation += offset;
		m_dirty = true;
	}

	void Positionable::setRotation(const glm::vec3& val)
	{
		m_rotation = val;
		m_dirty = true;
	}

	void Positionable::setTranslation(const glm::vec3& val)
	{
		m_translation = val;
		m_dirty = true;
	}
} // namespace ramen