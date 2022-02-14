#include "memory.h"

void Memory::Cell::backup()
{
    d_backupStack.push(Members{
                               identifier,
                               scope,
                               cellType,
                               size
        });

    cellType = CellSpec::RESERVED;
}

void Memory::Cell::restore()
{
    assert(d_backupStack.size() > 0 && "restore called on non-backupped cell");
    
    Members const &m = d_backupStack.top();
    identifier = std::get<0>(m);
    scope      = std::get<1>(m);
    cellType   = std::get<2>(m);
    size       = std::get<3>(m);

    d_backupStack.pop();
}
            
void Memory::Cell::clear()
{
    assert(cellType != CellSpec::RESERVED && "Cannot clear a reserved cell");
    
    identifier.clear();
    scope.clear();
    size = 0;
    cellType = CellSpec::EMPTY;
}
        
int Memory::findFree(int const sz)
{
    for (size_t start = 0; start != d_memory.size() - sz; ++start)
    {
        bool done = true;
        for (int offset = 0; offset != sz; ++offset)
        {
            if (!d_memory[start + offset].empty())
            {
                done = false;
                break;
            }
        }
        
        if (done) return start;
    }
    
    d_memory.resize(d_memory.size() + sz);
    return findFree(sz);
}

int Memory::getTemp(std::string const &scope, int const sz)
{
    int start = findFree(sz);
    Cell &cell = d_memory[start];

    cell.identifier = "";
    cell.scope = scope;
    cell.size = sz;
    cell.cellType = CellSpec::TEMP;

    for (int i = 1; i != sz; ++i)
    {
        Cell &cell = d_memory[start + i];
        cell.clear();
        cell.cellType = CellSpec::REFERENCED;
    }

    return start;
}

int Memory::getTempBlock(std::string const &scope, int const sz)
{
    int start = findFree(sz);
    for (int i = 0; i != sz; ++i)
    {
        Cell &cell = d_memory[start + i];
        cell.clear();
        cell.scope = scope;
        cell.size = 1;
        cell.cellType = CellSpec::TEMP;
    }

    return start;
}

int Memory::allocateLocal(std::string const &ident, std::string const &scope, int const sz)
{
    if (findLocal(ident, scope) != -1)
        return  -1;
    
    int addr = findFree(sz);
    Cell &cell = d_memory[addr];
    cell.clear();
    cell.identifier = ident;
    cell.scope = scope;
    cell.size = sz;
    cell.cellType = CellSpec::LOCAL;

    for (int i = 1; i != sz; ++i)
    {
        Cell &cell = d_memory[addr + i];
        cell.clear();
        cell.cellType = CellSpec::REFERENCED;
    }

    return addr;
}

int Memory::allocateGlobal(std::string const &ident, int const sz)
{
    if (findGlobal(ident) != -1)
        return  -1;
    
    int addr = findFree(sz);
    Cell &cell = d_memory[addr];
    cell.clear();
    cell.identifier = ident;
    cell.scope = "";
    cell.size = sz;
    cell.cellType = CellSpec::GLOBAL;
    
    for (int i = 1; i != sz; ++i)
    {
        Cell &cell = d_memory[addr + i];
        cell.clear();
        cell.cellType = CellSpec::REFERENCED;
    }

    return addr;
}

int Memory::findLocal(std::string const &ident, std::string const &scope)
{
    auto it = std::find_if(d_memory.begin(), d_memory.end(),
                           [&](Cell const &cell)
                           {
                               return scope.find(cell.scope) == 0 &&
                                   cell.identifier == ident;
                           });
    
    if (it != d_memory.end())
        return it - d_memory.begin();

    return -1;
}

int Memory::findGlobal(std::string const &ident)
{
    auto it = std::find_if(d_memory.begin(), d_memory.end(),
                           [&](Cell const &cell)
                           {
                               return cell.cellType == CellSpec::GLOBAL &&
                                   cell.identifier == ident;
                           });
    
    if (it != d_memory.end())
        return it - d_memory.begin();

    return -1;
}


void Memory::backup(int const addr)
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    d_memory[addr].backup();
}

void Memory::restore(int const addr)
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    d_memory[addr].restore();
}

void Memory::freeTemps(std::string const &scope)
{
    freeIf([&](Cell const &cell){
               return cell.cellType == CellSpec::TEMP &&
                   cell.scope == scope;
           });
}

void Memory::freeLocals(std::string const &scope)
{
    freeIf([&](Cell const &cell){
               return cell.scope == scope;
           });
}

int Memory::sizeOf(int const addr) const
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    Cell const &cell = d_memory[addr];
    assert(!cell.empty() && "Requested size of empty address");
    
    return d_memory[addr].size;
}

void Memory::markAsTemp(int const addr)
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    Cell &cell = d_memory[addr];
    cell.identifier = "";
    cell.cellType = CellSpec::TEMP;
}

void Memory::rename(int const addr, std::string const &ident, std::string const &scope)
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    Cell &cell = d_memory[addr];
    cell.identifier = ident;
    cell.scope = scope;
    cell.cellType = scope.empty() ? CellSpec::GLOBAL : CellSpec::LOCAL;
}

std::string Memory::identifier(int const addr) const
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    return d_memory[addr].identifier;
}

std::string Memory::scope(int const addr) const
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    return d_memory[addr].scope;
}

bool Memory::isTemp(int const addr) const
{
    assert(addr >= 0 && addr < (int)d_memory.size() && "address out of bounds");
    return d_memory[addr].cellType == CellSpec::TEMP;
}

