#ifndef DESCRIPTORTABLE_H
#define DESCRIPTORTABLE_H

namespace Dwarframe {
	
	class DescriptorTable
	{
	public:
		DescriptorTable();
		~DescriptorTable();

	private:
		D3D_ROOT_SIGNATURE_VERSION m_Version;
	};

}

#endif // !DESCRIPTORTABLE_H