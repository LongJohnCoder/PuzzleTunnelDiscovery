# SPDX-FileCopyrightText: Copyright © 2020 The University of Texas at Austin
# SPDX-FileContributor: Xinya Zhang <xinyazhang@utexas.edu>
# SPDX-License-Identifier: GPL-2.0-or-later
from .template import *

STICK_LENGTH = HOLLOW_SQUARE_SIZE + 2 * STICK_WIDTH

STICKS_X_DESC = [
        {
            'origin': (0,0),
            'len' : STICK_LENGTH,
            'up': [],
            'down': [0]
        },
        {
            'origin': (0, (HOLLOW_SQUARE_SIZE + STICK_WIDTH) * 1.0),
            'len' : STICK_LENGTH,
            'up': [],
            'down': []
        },
]

STICKS_Y_DESC = [
        {
            'origin': (0,0),
            'len' : STICK_LENGTH,
            'up': [],
            'down': [],
        },
        {
            'origin': ((HOLLOW_SQUARE_SIZE + STICK_WIDTH) * 1.0, 0.0),
            'len' : STICK_LENGTH,
            'up': [],
            'down': []
        },
]

