#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Dwarframe/Gameplay/Entity.h"

namespace Dwarframe {

	class Entity;
	class Material;
	class Mesh;
	class MaterialProxy;

	class Renderable
	{
	public:
		Renderable(Entity* RelatedGameObject);
		
		inline Entity* GetRelatedEntity() { return m_RelatedGameObject; }

		inline Material* GetMaterial() { return m_RelatedGameObject->m_Material; }
		inline Mesh* GetMesh() { return m_RelatedGameObject->m_Mesh; }

	private:
		Entity* m_RelatedGameObject; // Not sure if that's necessary maybe one directional communications is enough.
	};

}

#endif // !RENDERABLE_H
