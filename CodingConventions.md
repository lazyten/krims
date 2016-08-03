# Coding conventions
- This is up for discussion and not yet finalised.

## Use clang-format
This project makes use of a custom ``.clang-format`` file,
which defines the Code formatting used.
It is best to use ``clang-format`` together with this configuration file in
order to format your code before committing.

## Names of files, directories and code structures
### Code structures
- Use **CamelCase** for
	- ``class`` names
	- ``struct`` names
- Use **snake_case** for
	- ``namespace`` names
	- ``typedef``s or ``using``s

### File names
- Files containing exactly *one* ``class``: Use the name of the class in **CamelCase**,
  e.g. ``ClassName.hh``
- Files containin exactly one ``namespace``, where this file is the only one defining
  things of this namespace: **snake_case**, e.g. ``name_space.hh``
- Files which contain various ``class``es as some sort of a library
  (e.g. various error methods, various exceptions): **CamelCase**,
  e.g.  ``StuffLib.hh``
- Files which contain various definitions or ``typedef``s of the current namespace:
  **snake_case**, e.g. ``some_defs.hh``

### Folder names
- Folders which contain a full ``namespace``: Name of the ``namespace`` in
  **snake_case**: ``name_space``;
  optional: An extra file ``name_space.hh``, which includes the content files of
  the namespace.
- Folders which just contains a *collection of code*, which is in the same namespace
  as the parent folder: Use a sensible name in **CamelCase**, 
  e.g. ``CollectionStuff``.
  There also *has to* exist a file ``CollectionStuff.hh``, which includes all
  constituent headers inside the folder ``CollectionStuff``.
