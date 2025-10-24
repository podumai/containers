# Containers

## Tasks

1. Implement two dynamically sized containers—a sequential and a list-type container—and user code demonstrating the capabilities of these containers.
  * A distinctive feature of a sequential container is the arrangement of its elements one after another (like a regular C-style array).
  * A list-type container does not guarantee the arrangement of its elements in memory. Communication between elements is achieved through pointers (a pointer to the previous element and/or a pointer to the next element).

  The user code (called from the main function) must contain the following set of actions with both containers:

    1. Create a container object for storing int objects.
    2. Add ten elements (0, 1...9) to the container.
    3. Display the contents of the container on the screen. Expected results: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9.
    4. Display the size of the container on the screen. Expected result: 10.
    5. Remove the third (in a row), fifth, and seventh elements.
    6. Display the contents of the container. Expected result: 0, 1, 3, 5, 7, 8, 9.
    7. Add element 10 to the beginning of the container.
    8. Display the contents of the container. Expected result: 10, 0, 1, 3, 5, 7, 8, 9.
    9. Add element 20 to the middle of the container.
    10. Display the contents of the container. Expected result: 10, 0, 1, 3, 20, 5, 7, 8, 9.
    11. Add element 30 to the end of the container.
    12. Display the contents of the container. Expected result: 10, 0, 1, 3, 20, 5, 7, 8, 9, 30.

  Minimum interface requirements:

    * Method(s) (may be multiple) for adding elements to the end of the container ( push_back ).
    * Method(s) for inserting elements at an arbitrary position ( insert ).
    * Method(s) for removing elements from the container ( erase ).
    * Method(s) for getting the current size of the container ( size ).
    * Method(s) for accessing by index ( operator[] ).

    It is permissible to add other helper methods with custom code demonstrating their purpose.

2. Implement a sequential container with a small memory reservation upfront. Thus, when adding a new element, the container first creates a memory area larger than the required size to accommodate the elements (e.g., 50%). This way, subsequent additions of new elements can avoid re-allocating memory and copying. Note: Allocating memory too much is a bad idea, as it will lead to significant memory overhead. Typically, factors of 1.5, 1.6, and 2.0 are used.

3. Implement two variants of a list container:

  * Doubly linked list – each node stores a pointer to the next element and a pointer to the previous element.
  * Singly linked list – each node stores a pointer to the next element only.

4. Use move semantics in the implementation:

  * Implement a move constructor and assignment operator for containers.
  * Accept an r-value reference to a user-defined object as input.

5. Implement an additional structure – an iterator – that encapsulates the logic for traversing the container. Implement the following methods on the container:

  * begin() – returns an iterator to the beginning of the container
  * end() – returns an iterator to the end of the container

  Implement the following methods on the iterator:

    * operator*() – unary dereference operator
    * and/or the get() method to retrieve the value.

6. Build releases in the repository using CI/CD for at least two operating systems per commit.
