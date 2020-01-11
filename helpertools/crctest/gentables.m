% gentables.m
% 
% Generates three C arrays: one contains random messages, and the other two
% contain the encoded messages using RS(32,28) and RS(28,24).
%--------------------------------------------------------------------------
% Parameters
numbytes = 24;
messages = 100;

%--------------------------------------------------------------------------
% Generate random message
randombytes = zeros(messages,numbytes);
for i = 1:messages;
  randombytes(i,:) = uint8( floor( 256*rand(1,numbytes)));
end

% Encode the message using RS(24,28)
msg = zeros(messages, numbytes+4);
for i = 1:messages;
  enc1 = rsEncoder( randombytes(i,:), 8, 285, 28, 24 );
  msg(i,:) = enc1.x;
end

%--------------------------------------------------------------------------
% Output to file
file = fopen('testtables.h','w');
fprintf( file,'const uint8_t cau8RandomMessage[][24] = {\n' );
for k = 1:messages;
    fprintf( file, '{ %u', randombytes(k,1) );
    for i = 2:length( randombytes(1,:) );
      fprintf( file, ', %u', randombytes(k,i) );
    end
    fprintf( file, ' },\n' );
end
fprintf( file, ' };\n\n' );
%
fprintf( file,'const uint8_t cau8EncodedMessage[][28] = {\n' );
for k = 1:messages;
    fprintf( file, '{ %u', msg(k,1) );
    for i = 2:length( msg(1,:) );
      fprintf( file, ', %u', msg(k,i) );
    end
    fprintf( file, ' },\n' );
end
fprintf( file, ' };\n\n' );

fclose(file);
