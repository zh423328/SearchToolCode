---����ļ���Ҫ����Ѱ����Ҫ����������--
i = 0;

function DoFile(filepath,filepathto,filepathpathtxt)
	--��ȡ�ļ�--

	local BUFSIZE = 2^10 ;		--1k
	local file = assert(io.open(filepath,"r"));
	local fileto = io.open(filepathto,"w");
	local filetotxt = io.open(filepathpathtxt,"a+");

	if file then

		while true do

			----
			local str = file:read("*line");

			--ĩβ--
			if not str then break end

			str = str .. "\n";

			--local pat = '"%s-[^"]-[\128-\255]+[^"]-[^.][^h-H]%s-"';

		--�ַ����Һ��滻(-�������ٵ��滻)
			local newstr = string.gsub(str,'"%s-[^"]-[\161-\254]+[^"]-%s-"',
					function(x)
					filetotxt:write(string.format("%d\t%s\n",i+1,string.sub(x,2,-2)));
					i=i+1;
					return string.format("GAMETXT->GetGameTxt(%d)",i)
					end );

			fileto:write(newstr);
		end
		
		file:close();
		fileto:close();
		filetotxt:close();
	end
end


--DoFile("C:\1\1.txt","C:\2\1.txt","C:\2\chinese.txt");

--[[function extractChinese(s)
    for m in string.gmatch(s,'"[\176-\254]+"') do
        print(m)
    end
end
ss = 'foo"����"a"���"foo'
extractChinese(ss)--]]