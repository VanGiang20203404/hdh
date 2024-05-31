# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(create-exists) begin
(create-exists) create quux.dat
(create-exists) create warble.dat
EOF
pass;
