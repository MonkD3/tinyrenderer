# tinyrenderer

My *pure* `C` implementation of [ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer).


## API differences

This section lists the API and design differences between the original `C++` implementation of the TGA file format implementation.
In general, I have tried to minimize the differences both in terms of naming and code structure. 

### Differences due to language features

Of course, `C` does not have the Object Oriented capabilities of `C++`. To adapt `C++` OOP code towards `C` imperative implementation I have done the following. Imagine an object with private/protected and public fields and methods.

```cpp
class Object {
    private:
        private_field fpriv;
        void private_func(args);
    protected:
        protected_field fprot;
        void protected_func(args);
    public:
        public_field fpub;
        void public_func(args);
}
```

It is then transformed into the following structure and functions. Note that in the resulting `C` code, all fields and functions become public.

```c 
typedef struct {
    private_field fpriv;
    protected_field fprot;
    public_field fpub;
} Object_t;

void Object_private_func(Object_t* obj, args);
void Object_protected_func(Object_t* obj, args);
void Object_public_func(Object_t* obj, args);
```

This naming convention leads to the following changes when calling the methods :

```cpp 
Object obj = new Object();
obj.method(args);
```

```c 
Object_t obj;
Object_method(&obj, args);
```

#### Constructors/destructors

As `C++` allows for multiple constructors, they have been replaced by multiple functions with a description of what the constructor does. The destructor is simply a name change.
```c 
// The constructor Object(args) is replaced by
Object_init_<description>(Object_t* obj, args);

// The desctructor is replaced by
Object_destroy(Object_t* obj);
```

Note that the constructor using another instance of the object :
```cpp 
Object(const Object &o) : f1(o.f1), f2(o.f2) ... { }
```

Has been replaced by the `Object_copy(...)` function (see [below](#Operator overloading))

#### Operator overloading 

The operator overloading of `=` has been modified to a `void Object_copy(Object_t* o1, Object_t* o2);` function.

#### Default arguments 

All default arguments are now required. This impacts the function :
```cpp 
	bool write_tga_file(const char *filename, bool rle=true);
```

which becomes (see naming convention above)
```c 
bool TGAImage_write_tga_file(TGAImage_t* img, const char* filename, bool rle);
```

#### Getter/setter 

Trivial getter and setter where completely removed as all structure fields are now public.

### Design differences 


#### Structure modification

The initial `TGAColor` structure's `bytespp` field was removed and the resulting `TGAColor_t` is actually just a union whose purpose is to help reading the `data` field of `TGAImage_t`.

#### Types 

Numeric fields and variables now use `uint8_t, int32_t ...` from `<stdint.h>` with the notable exception of strings, which remain as `char*` for readability.

#### Qualifiers 

The `const` qualifier was added where it made sense.

#### Return types 

All returned objects are now pointers to the object. For example 

```cpp
TGAColor TGAImage::get(int x, int y);
```
```c
TGAColor_t* TGAImage_get(TGAImage_t const * const img, int32_t const x, int32_t const y);
```
