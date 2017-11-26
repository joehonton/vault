# Vault
*Object oriented access to SQL databases*

VAULT is a C++ library that provides silent methods for retrieving, caching, and reserializing the properties of your classes.

## Problem Statement

Classic ORDBMS approaches require you to explicitly code each attempt to access a database record set. This is not necessary. Instead, this library uses auto-save in the destructor, and auto-retrieve in the constructor to move bytes from an SQL store into your class instance.

## License

Bequiesce is open-sourced under the
![](http://bequiesce.com/img/CC-BY-NC-ND.png)
[CC-BY-NC-ND](http://htmlpreview.github.com/?https://github.com/joehonton/vault/blob/master/LICENSE.html)
License.

