efm;  % loads EFM code table

start = 1122;  % first edge to trigger
bittime = 1000000000/(2*4321800);  % 1 GS/sec per two times the channel baud rate (2x CD-ROM)

rfproc = sign(filter(FiltNum,1,rf(1:1000000)));  % rf contains the samples
%rfproc = sign(rf(1:1000000));  % rf contains the samples
bitmatrix = 1;  % this will be used for storing the sampled bits

% EFM sync pattern
syncpattern = [1 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 0]';

% NRZI decoding with synchronization and filtering
lastbit = start;
samplepoints = start;
for i = start:(length(rfproc)-1);
  if( rfproc(i) ~= rfproc(i+1) )  % if there is an edge
    temp = round( (i-lastbit)/bittime );
    if( temp >= 1 )  % if it's a valid bit
      bitmatrix = cat(1,bitmatrix(1:end),1);
      bitmatrix = cat(1,bitmatrix(1:end),zeros(temp-1,1));
      samplepoints = cat(1,samplepoints,lastbit);
      for n=1:(temp-1);
        samplepoints = cat(1,samplepoints, lastbit+n*bittime);
      end
      lastbit = i;
    end
  end
end

% visualize -- for debug
%plot(rf(1:1000000));hold on;
plot(rfproc(750:end));hold on;
plot(samplepoints-750*ones(length(samplepoints),1),bitmatrix,'*');

% Search for sync pattern
syncend = 1;
for i = 1:(length(bitmatrix)-length(syncpattern));
  if( isequal(syncpattern, bitmatrix(i:(i+length(syncpattern)-1)) ) )
     syncend = i+length(syncpattern)+3;  % 3 = merging bits
     break;
  end
end

% EFM decoding from the first sync pattern (F3 frame)
F3Frames = [];
for k = 1:floor(length(bitmatrix(syncend:end))/588);
  bytestream = [];
  for i = syncend:17:(syncend-1+14*floor(length(bitmatrix(syncend:end))/14));
    temp = 0;
    for n = 1:length(EFMCodeTable(:,1));
      if( isequal( EFMCodeTable(n,:)', bitmatrix(i:(i+13)) ) )
        temp = 1;
        bytestream = cat( 1, bytestream, n );
        break;
      end
    end
    if( 0 == temp )  % if it does not match any 14-bit words
      if( isequal(syncpattern, bitmatrix((i-2):(i-2+length(syncpattern)-1)) ) )
        % it's a new sync word, restart stream
        F3Frames = cat(2,F3Frames, bytestream);
        syncend = (i-2+length(syncpattern))+3;
        break;  % next frame
      else
        error('Cannot find matching word!');
        %bytestream = cat( 1, bytestream, -1 );
      end
    end
  end
end

