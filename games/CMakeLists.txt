# Author: petter.strandmark@gmail.com (Petter Strandmark)

MACRO (CREATE_EXAMPLE NAME)
	ADD_EXECUTABLE(${NAME}
	               ${NAME}.cpp
	               ${MCTS_HEADERS})
	TARGET_LINK_LIBRARIES( ${NAME} pthread )
	MESSAGE("-- Adding game: " ${NAME})
ENDMACRO (CREATE_EXAMPLE)

CREATE_EXAMPLE(connect_four)
CREATE_EXAMPLE(nim)
CREATE_EXAMPLE(gobang)

