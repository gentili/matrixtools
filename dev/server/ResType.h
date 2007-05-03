#ifndef RESTYPE_H
#define RESTYPE_H

typedef enum ResTypeEnum {
	RT_undefined = 0
} ResType_e;

class ResType {
public:
	ResType(ResType_e restype) { _restype = restype; }
	
	ResType_e get_restype();
private:
	ResType_e _restype;
};

#endif
