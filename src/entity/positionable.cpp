#include "positionable.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../perfmon/profiler.h"

namespace ramen
{
	const uint32_t Positionable::CompoID_Positionable = 0x00000001;

	Positionable::Positionable()
		: m_dirty(true)
		, m_scale(1.f)
	{
	}

	void Positionable::update()
	{
		if (m_dirty) {
			PROFILE;
			glm::vec3 angles(glm::radians(m_rotation.x), glm::radians(m_rotation.y), glm::radians(m_rotation.z));
			glm::quat quat(angles);

			m_model = glm::mat4x4(1.f);
			m_model = glm::scale(m_model, m_scale);
			m_model *= glm::toMat4(quat);
			m_model = glm::translate(m_model, m_translation);
			m_dirty = false;
		}
	}

	void Positionable::setTranslation(const glm::vec3& translation)
	{
		m_translation = translation;
		m_dirty = true;
	}
} // namespace ramen