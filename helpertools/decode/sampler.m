efm;  % loads EFM code table

start = 2072;  % first edge to trigger
bittime = 1000000000/(4321800);  % 1 GS/sec per channel baud rate (1x CD-ROM)

%rfproc = sign(filter(FiltNum,1,rf(1:1000000)));  % rf contains the samples
rfproc = sign(rf(1:end));  % rf contains the samples

% EFM sync pattern
syncpattern = [1 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 0]';

% NRZI decoding with synchronization and filtering
bitmatrix = 1;  % this will be used for storing the sampled bits
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

% % visualize -- for debug
% plot(rf(1:end));hold on;
% plot(rfproc(1:end));hold on;
% plot(samplepoints-0*ones(length(samplepoints),1),bitmatrix,'*');

% Search for sync pattern
syncend = 1;
for i = 1:(length(bitmatrix)-length(syncpattern));
  if( isequal(syncpattern, bitmatrix(i:(i+length(syncpattern)-1)) ) )
     syncend = i+length(syncpattern)+3;  % 3 = merging bits
     break;
  end
end

% EFM decoding from the first sync pattern (F3 frame)
nextframe = 0;
maxframes = 0;
F3Frames = [];
for k = 1:floor(length(bitmatrix(syncend:end))/588);
  bytestream = [];
  for i = syncend:17:(syncend-1+14*floor(length(bitmatrix(syncend:end))/14));
    temp = 0;
    if( isequal(syncpattern, bitmatrix((i-2):(i-2+length(syncpattern)-1)) ) )
      % it's a new sync word, restart stream
      F3Frames = cat(2,F3Frames, bytestream);
      syncend = (i-2+length(syncpattern))+3;
      break;  % next frame
    else
      for n = 1:length(EFMCodeTable(:,1));
        if( isequal( EFMCodeTable(n,:)', bitmatrix(i:(i+13)) ) )
          if( ( 90 == n ) && (isequal(syncpattern, bitmatrix((i):(i+length(syncpattern)-1)) ) ) )
            % it's a new sync word, restart stream
            F3Frames = cat(2,F3Frames, bytestream);
            syncend = (i+length(syncpattern))+3;
            bytestream = [];
            temp = 1;
            nextframe = 1;
            break;  % next frame
          else
            temp = 1;
            bytestream = cat( 1, bytestream, n-1 );
            break;
          end
        end
      end
      if( 0 ~= nextframe )
        nextframe = 0;
        break;
      end
    end
    if( 0 == temp )  % if it does not match any 14-bit words
      %error('Cannot find matching word!');
      %bytestream = cat( 1, bytestream, -1 );
      if( ~isequal(size(F3Frames),[0 0]) && (length(F3Frames(1,:)) >= maxframes ) )
        maxframes = length(F3Frames(1,:));
      end
      if( ~isequal(size(F3Frames),[0 0]) && (length(F3Frames(1,:)) >= 192 ) )
        syncend = length(bitmatrix);
        break;
      end
      % resynchronize
      for n = syncend:(length(bitmatrix)-length(syncpattern));
        if( isequal(syncpattern, bitmatrix(n:(n+length(syncpattern)-1)) ) )
           syncend = n+length(syncpattern)+3;  % 3 = merging bits
           break;
        end
      end
      F3Frames = [];
      break;
    end
  end
end

