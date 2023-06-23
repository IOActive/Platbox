import ctypes 

_u8 = ctypes.c_uint8
_u16 = ctypes.c_uint16
_u32 = ctypes.c_uint32
_u64 = ctypes.c_uint64

u8 = _u8
u16 = _u16
u32 = _u32
u64 = _u64

_g_formats = {
	_u8		: "0x{:02x}",
	_u16	: "0x{:04x}",
	_u32	: "0x{:08x}",
	_u64	: "0x{:016x}",
}

def w32(dst_obj, src_obj):
	"""Copies a 32-bit value from src obj, and assignes it to the value of dst_obj."""
	dst_obj.value = _u32.from_address(ctypes.addressof(src_obj)).value

def w64(dst_obj, src_obj):
	"""Copies a 64-bit value from src obj, and assignes it to the value of dst_obj."""
	dst_obj.value = _u64.from_address(ctypes.addressof(src_obj)).value
	
def as32(obj):
	"""Copies the first 32-bits of the object and returns it as a number."""
	return _u32.from_address(ctypes.addressof(obj)).value
	
def as64(obj):
	"""Copies the first 64-bits of the object and returns it as a number."""
	return _u64.from_address(ctypes.addressof(obj)).value


class PrintingStructure(ctypes.Structure):

	def value_to_string(self, t, v, bits=None):
		if isinstance(v, bytes):
			return repr(v)
		elif issubclass(t, ctypes.Array):
			elements = []
			for i in v:
				elements.append(self.value_to_string(v._type_, i))
			return "[{}]".format(", ".join(elements))
		else:
			if bits != None:
				if bits <= 8:
					t = _u8
				elif bits <= 16:
					t = _u16
				elif bits <= 32:
					t = _u32
				else:
					t = _u64
			fmt = _g_formats.get(t, "{}")
			return fmt.format(v)
			

	def show(self, prefix="", top_level=True):
		if top_level:
			print("{}:".format(self.__class__.__name__))
			prefix += " "
		for field in self._fields_:
			name = field[0]
			field_type = field[1]
			value = getattr(self, name)
			if issubclass(field_type, PrintingStructure):
				print("{}{}:".format(prefix, name))
				value.show(prefix + " ", top_level=False)
			else:
				value_str = self.value_to_string(field_type, value, field[2] if len(field) > 2 else None)
				print("{}{}: {}".format(prefix, name, value_str))
	
	def sizeof(self):
		return ctypes.sizeof(self)